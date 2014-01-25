//
// Vortexje -- Clark-Y airfoil generator.
//
// Copyright (C) 2012 - 2014 Baayen & Heinz GmbH.
//
// Authors: Jorn Baayen <jorn.baayen@baayen-heinz.com>
//

#include <iostream>

#include <vortexje/airfoils/clark-y.hpp>

using namespace std;
using namespace Eigen;
using namespace Vortexje::Airfoils;

// Clark-Y airfoil generation:
#define CLARKY_DATA_SIZE 61

static struct {
    double x;
    double y;
} clarky_upper_data[CLARKY_DATA_SIZE] = {
    {0.0000000, 0.0000000},
    {0.0005000, 0.0023390},
    {0.0010000, 0.0037271},
    {0.0020000, 0.0058025},
    {0.0040000, 0.0089238},
    {0.0080000, 0.0137350},
    {0.0120000, 0.0178581},
    {0.0200000, 0.0253735},
    {0.0300000, 0.0330215},
    {0.0400000, 0.0391283},
    {0.0500000, 0.0442753},
    {0.0600000, 0.0487571},
    {0.0800000, 0.0564308},
    {0.1000000, 0.0629981},
    {0.1200000, 0.0686204},
    {0.1400000, 0.0734360},
    {0.1600000, 0.0775707},
    {0.1800000, 0.0810687},
    {0.2000000, 0.0839202},
    {0.2200000, 0.0861433},
    {0.2400000, 0.0878308},
    {0.2600000, 0.0890840},
    {0.2800000, 0.0900016},
    {0.3000000, 0.0906804},
    {0.3200000, 0.0911857},
    {0.3400000, 0.0915079},
    {0.3600000, 0.0916266},
    {0.3800000, 0.0915212},
    {0.4000000, 0.0911712},
    {0.4200000, 0.0905657},
    {0.4400000, 0.0897175},
    {0.4600000, 0.0886427},
    {0.4800000, 0.0873572},
    {0.5000000, 0.0858772},
    {0.5200000, 0.0842145},
    {0.5400000, 0.0823712},
    {0.5600000, 0.0803480},
    {0.5800000, 0.0781451},
    {0.6000000, 0.0757633},
    {0.6200000, 0.0732055},
    {0.6400000, 0.0704822},
    {0.6600000, 0.0676046},
    {0.6800000, 0.0645843},
    {0.7000000, 0.0614329},
    {0.7200000, 0.0581599},
    {0.7400000, 0.0547675},
    {0.7600000, 0.0512565},
    {0.7800000, 0.0476281},
    {0.8000000, 0.0438836},
    {0.8200000, 0.0400245},
    {0.8400000, 0.0360536},
    {0.8600000, 0.0319740},
    {0.8800000, 0.0277891},
    {0.9000000, 0.0235025},
    {0.9200000, 0.0191156},
    {0.9400000, 0.0146239},
    {0.9600000, 0.0100232},
    {0.9700000, 0.0076868},
    {0.9800000, 0.0053335},
    {0.9900000, 0.0029690},
    {1.0000000, 0.0005993}
};

static struct {
    double x;
    double y;
} clarky_lower_data[CLARKY_DATA_SIZE] = {
    {0.0000000, 0.0000000},
    {0.0005000, -.0046700},
    {0.0010000, -.0059418},
    {0.0020000, -.0078113},
    {0.0040000, -.0105126},
    {0.0080000, -.0142862},
    {0.0120000, -.0169733},
    {0.0200000, -.0202723},
    {0.0300000, -.0226056},
    {0.0400000, -.0245211},
    {0.0500000, -.0260452},
    {0.0600000, -.0271277},
    {0.0800000, -.0284595},
    {0.1000000, -.0293786},
    {0.1200000, -.0299633},
    {0.1400000, -.0302404},
    {0.1600000, -.0302546},
    {0.1800000, -.0300490},
    {0.2000000, -.0296656},
    {0.2200000, -.0291445},
    {0.2400000, -.0285181},
    {0.2600000, -.0278164},
    {0.2800000, -.0270696},
    {0.3000000, -.0263079},
    {0.3200000, -.0255565},
    {0.3400000, -.0248176},
    {0.3600000, -.0240870},
    {0.3800000, -.0233606},
    {0.4000000, -.0226341},
    {0.4200000, -.0219042},
    {0.4400000, -.0211708},
    {0.4600000, -.0204353},
    {0.4800000, -.0196986},
    {0.5000000, -.0189619},
    {0.5200000, -.0182262},
    {0.5400000, -.0174914},
    {0.5600000, -.0167572},
    {0.5800000, -.0160232},
    {0.6000000, -.0152893},
    {0.6200000, -.0145551},
    {0.6400000, -.0138207},
    {0.6600000, -.0130862},
    {0.6800000, -.0123515},
    {0.7000000, -.0116169},
    {0.7200000, -.0108823},
    {0.7400000, -.0101478},
    {0.7600000, -.0094133},
    {0.7800000, -.0086788},
    {0.8000000, -.0079443},
    {0.8200000, -.0072098},
    {0.8400000, -.0064753},
    {0.8600000, -.0057408},
    {0.8800000, -.0050063},
    {0.9000000, -.0042718},
    {0.9200000, -.0035373},
    {0.9400000, -.0028028},
    {0.9600000, -.0020683},
    {0.9700000, -.0017011},
    {0.9800000, -.0013339},
    {0.9900000, -.0009666},
    {1.0000000, -.0005993}
};

// Cosine rule:
static double
cosine_rule(int n_points, int i)
{
    return 0.5 * (1 - cos(M_PI * i / (double) n_points));
}

/**
   Generates points tracing a Clark-Y airfoil.
   
   @param[in]   chord                    Chord length.
   @param[in]   n_points                 Number of points to return.
   @param[out]  trailing_edge_point_id   Index of the trailing edge node in the returned list.
   
   @returns List of points.
*/
vector<Vector3d, Eigen::aligned_allocator<Vector3d> >
ClarkY::generate(double chord, int n_points, int &trailing_edge_point_id)
{
    if (n_points % 2 == 1) {
        cerr << "ClarkY::generate(): n_nodes must be even." << endl;
        exit(1);
    }
    
    vector<Vector3d, Eigen::aligned_allocator<Vector3d> > airfoil_points;
    
    // Add upper nodes:
    for (int i = 0; i < n_points / 2; i++) {
        double x = cosine_rule(n_points / 2, i);
        
        int clarky_upper_idx;
        for (clarky_upper_idx = 0; clarky_upper_idx < CLARKY_DATA_SIZE; clarky_upper_idx++) {
            if (x <= clarky_upper_data[clarky_upper_idx].x)
                break;
        }
        
        double y;
        if (clarky_upper_idx == 0)
            y = clarky_upper_data[0].y;
        else
            y = clarky_upper_data[clarky_upper_idx - 1].y + (clarky_upper_data[clarky_upper_idx].y - clarky_upper_data[clarky_upper_idx - 1].y) / (clarky_upper_data[clarky_upper_idx].x - clarky_upper_data[clarky_upper_idx - 1].x) * (x - clarky_upper_data[clarky_upper_idx - 1].x);
        
        Vector3d upper_point(chord * x, chord * y, 0.0);
        airfoil_points.push_back(upper_point);
    }
    
    // Add lower nodes:
    for (int i = 0; i < n_points / 2; i++) {
        double x = 1 - cosine_rule(n_points / 2, i);
        
        int clarky_lower_idx;
        for (clarky_lower_idx = 0; clarky_lower_idx < CLARKY_DATA_SIZE; clarky_lower_idx++) {
            if (x <= clarky_lower_data[clarky_lower_idx].x)
                break;
        }
        
        double y;
        if (clarky_lower_idx == 0)
            y = clarky_lower_data[0].y;
        else
            y = clarky_lower_data[clarky_lower_idx - 1].y + (clarky_lower_data[clarky_lower_idx].y - clarky_lower_data[clarky_lower_idx - 1].y) / (clarky_lower_data[clarky_lower_idx].x - clarky_lower_data[clarky_lower_idx - 1].x) * (x - clarky_lower_data[clarky_lower_idx - 1].x);
        
        Vector3d lower_point(chord * x, chord * y, 0.0);
        airfoil_points.push_back(lower_point);
    }
    
    // Done.
    trailing_edge_point_id = n_points / 2;
    
    return airfoil_points;
}
