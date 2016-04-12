#!/usr/bin/python
import numpy as np
import math

#get the camera calibration matrix
#focus is the camera's focus in distance units
#principle is the priniple point of the camera (usually width/2, height/2)
#scale is the conversion factor pixel per distance unit
def getCalibMatrix(focus,principle,scale=1,skew=0):
    return np.matrix([[focus*scale[0],skew,principle[0]],
                      [0,focus*scale[1],principle[1]],
                      [0,0,1]])

#this function gets the rotation matrix from euclidean rotations
#rotx, roty, rotz correspond to the x y z axial rotations
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

#this function is a convenience function for packing the image coordinates and focus into a column matrix
def imgToCam(focus,coord_img,scale=1):
    return np.matrix([[coord_pic[0]],[coord_pic[1]],focus]])

#this function transforms coordinates on the camera plane into vehicular coordinates
#get calib_matrix from the getCalibMatrix function
#rot_matrix from the getRotMatrix function
#cam_pos should be predetermined through measurement
#coord_cam from imgToCam
#depth is the vehicle altitude - ground altitude
#Camera Plane: principal point is 0,0, +x is left to right, +y is up to down
#Vehicle Space: +x is back to forward, +z is down to up, +y is right to left
def camToVehicle(calib_matrix,rot_matrix,cam_pos,coord_cam,depth):
    #C = -R*T
    #T = -Rt*T
    #get the translation usting T=-Rt*t
    T = -rot_matrix.getT()*cam_pos
    #get the conversion matrix KRT through multiplication
    KRT = calib_matrix * rot_matrix * T
    #get the homogenous representation of the coordinates in the vehicle system
    coord_vehicle = np.linalg.inv(KRT.getT()*KRT)*KRT.getT()*coord_cam
    #scale the Z 
    coord_vehicle = coord_vehicle*-depth/coord_vehicle[2,0]
    return coord_vehicle

#this function transforms coordinates from the vehicle coordinate system to lat,long,altitude
def vehicleToWorld(rot_matrix,vehicle_pos,coord_vehicle):
    T = -rot_matrix.getT()*cam_pos
    KRT = calib_matrix * rot_matrix * T
    coord_vehicle = np.linalg.inv(KRT.getT()*KRT)*KRT.getT()*coord_cam
    coord_vehicle = coord_vehicle*altitude/coord_vehicle[2,0]
    return coord_vehicle

if __name__ == "__main__":
    print "Hello, World!"
