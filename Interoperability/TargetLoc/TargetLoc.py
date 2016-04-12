#!/usr/bin/python
import numpy as np
import math

#get the camera calibration matrix
def getCalibMatrix(focus,principle,scale=[1,1],skew=1):
    return np.matrix([[focus*scale[0],skew,principle[0]],
                      [0,focus*scale[1],principle[1]],
                      [0,0,1]])

#get the rotation matrix from euclidean rotations x,y,z
def getRotMatrix(rotx,roty,rotz):
    cosx=math.cos(rotx)
    sinx=math.sin(rotx)
    cosy=math.cos(roty)
    siny=math.sin(roty)
    cosz=math.cos(rotz)
    sinz=math.sin(rotz)
    x_matrix = np.matrix([[1,0,0],
                          [0,cosx,-sinx],
                          [0,sinx,cosx]])
    y_matrix = np.matrix([[cosy,0,-siny],
                          [0,1,0],
                          [siny,0,cosy]])
    z_matrix = np.matrix([[cosz,-sinz,0],
                          [sinz,cosz,0],
                          [0,0,1]])
    return x_matrix*y_matrix*z_matrix

#this method transforms coordinates on the camera plane into vehicular coordinates
#Camera Plane: principal point is 0,0, +x is left to right, +y is up to down
#Vehicle Space: +x is back to forward, +z is down to up, +y is right to left
def camToVehicle(calib_matrix,rot_matrix,cam_pos,coord_cam,altitude):
    #C = -R*T
    #T = -Rt*T
    #get the translation usting T=-Rt*t
    T = -rot_matrix.getT()*cam_pos
    #get the conversion matrix KRT through multiplication
    KRT = calib_matrix * rot_matrix * T
    #get the homogenous representation of the coordinates in the vehicle system
    coord_vehicle = np.linalg.inv(KRT.getT()*KRT)*KRT.getT()*coord_cam
    #scale the Z 
    coord_vehicle = coord_vehicle*-altitude/coord_vehicle[2,0]
    return coord_vehicle

#this method transforms coordinates from the vehicle coordinate system to lat,long,altitude
def vehicleToWorld(rot_matrix,vehicle_pos,coord_vehicle):
    T = -rot_matrix.getT()*cam_pos
    KRT = calib_matrix * rot_matrix * T
    coord_vehicle = np.linalg.inv(KRT.getT()*KRT)*KRT.getT()*coord_cam
    coord_vehicle = coord_vehicle*altitude/coord_vehicle[2,0]
    return coord_vehicle

if __name__ == "__main__":
    print "Hello, World!"
