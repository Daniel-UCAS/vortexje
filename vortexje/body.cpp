//
// Vortexje -- Body.
//
// Copyright (C) 2012 - 2014 Baayen & Heinz GmbH.
//
// Authors: Jorn Baayen <jorn.baayen@baayen-heinz.com>
//

#include <vortexje/body.hpp>
#include <vortexje/boundary-layers/dummy-boundary-layer.hpp>

#include <iostream>
#include <assert.h>

using namespace std;
using namespace Eigen;
using namespace Vortexje;

/**
   Constructs a new Body.
   
   @param[in]   id   Name for this body.
*/
Body::Body(const string &id) : id(id)
{
    // Initialize kinematics:
    position = Vector3d(0, 0, 0);
    velocity = Vector3d(0, 0, 0);
    
    attitude = Quaterniond(1, 0, 0, 0);
    rotational_velocity = Vector3d(0, 0, 0);
}

/**
   Body destructor.
*/
Body::~Body()
{
    vector<SurfaceData*>::iterator sdi;
    for (sdi = non_lifting_surfaces.begin(); sdi != non_lifting_surfaces.end(); sdi++)
        delete (*sdi);
        
    vector<LiftingSurfaceData*>::iterator lsdi;
    for (lsdi = lifting_surfaces.begin(); lsdi != lifting_surfaces.end(); lsdi++)
        delete (*lsdi);
        
    vector<Surface*>::iterator si;
    for (si = allocated_surfaces.begin(); si != allocated_surfaces.end(); si++)
        delete (*si);
        
    vector<BoundaryLayer*>::iterator bi;
    for (bi = allocated_boundary_layers.begin(); bi != allocated_boundary_layers.end(); bi++)
        delete (*bi);
}

/**
   Adds a non-lifting surface to this body.
   
   @param[in]   non_lifting_surface   Non-lifting surface.
*/
void
Body::add_non_lifting_surface(Surface &non_lifting_surface)
{
    BoundaryLayer *boundary_layer = new DummyBoundaryLayer();
    
    add_non_lifting_surface(non_lifting_surface, *boundary_layer);
    
    allocated_boundary_layers.push_back(boundary_layer);
}

/**
   Adds a non-lifting surface and its boundary layer to this body.
   
   @param[in]   non_lifting_surface   Non-lifting surface.
   @param[in]   boundary_layer        Boundary layer.
*/
void
Body::add_non_lifting_surface(Surface &non_lifting_surface, BoundaryLayer &boundary_layer)
{
    non_lifting_surfaces.push_back(new SurfaceData(non_lifting_surface, boundary_layer));
}

/**
   Adds a lifting surface to this body.
   
   @param[in]   lifting_surface   Lifting surface.
*/
void
Body::add_lifting_surface(LiftingSurface &lifting_surface)
{
    BoundaryLayer *boundary_layer = new DummyBoundaryLayer();
    Wake *wake = new Wake(lifting_surface);
    
    add_lifting_surface(lifting_surface, *boundary_layer, *wake);
    
    allocated_surfaces.push_back(wake);
    allocated_boundary_layers.push_back(boundary_layer);
}

/**
   Adds a lifting surface and its boundary layer to this body.
   
   @param[in]   lifting_surface   Lifting surface.
   @param[in]   boundary_layer    Boundary layer.
   @param[in]   wake              Wake.
*/
void
Body::add_lifting_surface(LiftingSurface &lifting_surface, BoundaryLayer &boundary_layer, Wake &wake)
{
    lifting_surfaces.push_back(new LiftingSurfaceData(lifting_surface, boundary_layer, wake));
}

/**
   Creates an across-surface neighborhood relationship between two panels.  This operation
   is also known as stitching.
   
   @param[in]   surface_a   First reference surface.
   @param[in]   panel_a     First reference panel.
   @param[in]   edge_a      First reference edge.
   @param[in]   surface_b   Second reference surface.
   @param[in]   panel_b     Second reference panel.
   @param[in]   edge_b      Second reference edge.
*/
void
Body::stitch_panels(const Surface &surface_a, int panel_a, int edge_a, const Surface &surface_b, int panel_b, int edge_b)
{
    // Add stitch from A to B:
    stitches[SurfacePanelEdge(surface_a, panel_a, edge_a)].push_back(SurfacePanelEdge(surface_b, panel_b, edge_b));

    // Add stitch from B to A:
    stitches[SurfacePanelEdge(surface_b, panel_b, edge_b)].push_back(SurfacePanelEdge(surface_a, panel_a, edge_a));
}

/**
   Lists both in-surface and across-surface (stitched) neighbors of the given panel.
   
   @param[in]   surface   Reference surface.
   @param[in]   panel     Reference panel.
   
   @returns List of in-surface and across-surface panel neighbors.
*/
vector<Body::SurfacePanelEdge>
Body::panel_neighbors(const Surface &surface, int panel) const
{
    vector<SurfacePanelEdge> neighbors;
    
    // List in-surface neighbors:
    for (int i = 0; i < (int) surface.panel_nodes[panel].size(); i++) {
        map<int, int>::const_iterator isit = surface.panel_neighbors[panel].find(i);
        if (isit != surface.panel_neighbors[panel].end()) {            
            int neighbor_panel = isit->second;
        
            neighbors.push_back(SurfacePanelEdge(surface, neighbor_panel, i));
        }
    }
    
    // List stitches:
    map<SurfacePanelEdge, vector<SurfacePanelEdge>, CompareSurfacePanel>::const_iterator it =
        stitches.find(SurfacePanelEdge(surface, panel, -1));
    if (it != stitches.end()) {
        vector<SurfacePanelEdge> neighbor_surface_panels = it->second; 
        
        vector<SurfacePanelEdge>::iterator sit;
        for (sit = neighbor_surface_panels.begin(); sit != neighbor_surface_panels.end(); sit++)
            neighbors.push_back(*sit);       
    }
    
    // Done:
    return neighbors;
}

/**
   Lists both in-surface and across-surface (stitched) neighbors of the given panel and edge.
   
   @param[in]   surface   Reference surface.
   @param[in]   panel     Reference panel.
   @param[in]   edge      Reference edge.
   
   @returns List of in-surface and across-surface panel neighbor for the given edge.
*/
vector<Body::SurfacePanelEdge>
Body::panel_neighbors(const Surface &surface, int panel, int edge) const
{
    vector<SurfacePanelEdge> neighbors;
    
    // List in-surface neighbor:
    map<int, int>::const_iterator isit = surface.panel_neighbors[panel].find(edge);
    if (isit != surface.panel_neighbors[panel].end()) {
        int neighbor_panel = isit->second;

        neighbors.push_back(SurfacePanelEdge(surface, neighbor_panel, edge));
    }
    
    // List stitches:
    map<SurfacePanelEdge, vector<SurfacePanelEdge>, CompareSurfacePanel>::const_iterator it =
        stitches.find(SurfacePanelEdge(surface, panel, -1));
    if (it != stitches.end()) {
        vector<SurfacePanelEdge> neighbor_surface_panels = it->second; 
        
        vector<SurfacePanelEdge>::iterator sit;
        for (sit = neighbor_surface_panels.begin(); sit != neighbor_surface_panels.end(); sit++)
            if (sit->edge == edge)
                neighbors.push_back(*sit);       
    }
    
    // Check number of neighbors:
    assert(neighbors.size() <= 1);
    
    // Done:
    return neighbors;
}

/**
   Sets the linear position of this body.
   
   @param[in]   position   Linear position.
*/
void
Body::set_position(const Vector3d &position)
{
    // Compute differential translation:
    Vector3d translation = position - this->position;
       
    // Apply:
    vector<SurfaceData*>::iterator si;
    for (si = non_lifting_surfaces.begin(); si != non_lifting_surfaces.end(); si++) {
        SurfaceData *d = *si;
        
        d->surface.translate(translation);
    }
    
    vector<LiftingSurfaceData*>::iterator lsi;
    for (lsi = lifting_surfaces.begin(); lsi != lifting_surfaces.end(); lsi++) {
        LiftingSurfaceData *d = *lsi;
        
        d->surface.translate(translation);
        
        d->wake.translate_trailing_edge(translation);
    }
    
    // Update state:
    this->position = position;
}

/**
   Sets the attitude (orientation) of this body.
   
   @param[in]   attitude   Attitude (orientation) of this body, as normalized quaternion.
*/
void
Body::set_attitude(const Quaterniond &attitude)
{   
    // Compute differential transformation:
    Transform<double, 3, Affine> transformation = Translation<double, 3>(position) * attitude * this->attitude.inverse() * Translation<double, 3>(-position);
    
    // Apply:
    vector<SurfaceData*>::iterator si;
    for (si = non_lifting_surfaces.begin(); si != non_lifting_surfaces.end(); si++) {
        SurfaceData *d = *si;
        
        d->surface.transform(transformation);
    }
    
    vector<LiftingSurfaceData*>::iterator lsi;
    for (lsi = lifting_surfaces.begin(); lsi != lifting_surfaces.end(); lsi++) {
        LiftingSurfaceData *d = *lsi;
        
        d->surface.transform(transformation);
        
        d->wake.transform_trailing_edge(transformation);
    }
    
    // Update state:
    this->attitude = attitude;
}

/**
   Sets the linear velocity of this body.
   
   @param[in]   velocity   Linear velocity.
*/
void 
Body::set_velocity(const Vector3d &velocity)
{
    this->velocity = velocity;
}

/**
   Sets the rotational velocity of this body.
   
   @param[in]   rotational_velocity   Rotational velocity.
*/
void
Body::set_rotational_velocity(const Vector3d &rotational_velocity)
{
    this->rotational_velocity = rotational_velocity;
}

/**
   Computes the kinematic velocity of the given panel.
   
   @param[in]   surface   Surface, belonging to this body. 
   @param[in]   panel     Panel, belonging to this surface.
   
   @return The kinematic velocity.
*/
Vector3d
Body::panel_kinematic_velocity(const Surface &surface, int panel) const
{
    const Vector3d &panel_position = surface.panel_collocation_point(panel, false);
    Vector3d r = panel_position - position;
    return velocity + rotational_velocity.cross(r);
}

/**
   Computes the kinematic velocity of the given node.
   
   @param[in]   surface   Surface, belonging to this body. 
   @param[in]   node      Node, belonging to this surface.
   
   @return The kinematic velocity.
*/
Vector3d
Body::node_kinematic_velocity(const Surface &surface, int node) const
{
    Vector3d r = surface.nodes[node] - position;
    return velocity + rotational_velocity.cross(r);
}
