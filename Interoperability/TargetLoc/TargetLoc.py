#!/usr/bin/python
import numpy as np
import math

#get the camera calibration matrix
#focus is the camera's focus in distance units
#principle is the priniple point of the camera (usually width/2, height/2)
#scale is the conversion factor pixel per distance unit
def getCalibMatrix(focal_len,principle,scale=1,skew=0):
    return np.matrix([[focal_len*scale,skew,principle[0]],
                      [0,focal_len*scale,principle[1]],
                      [0,0,1]])

#this function gets the rotation matrix from euclidean rotations
#rotx, roty, rotz correspond to the x y z axial rotations
def getRotMatrix(rot_x,rot_y,rot_z):
    cosx=math.cos(rot_x)
    sinx=math.sin(rot_x)
    cosy=math.cos(rot_y)
    siny=math.sin(rot_y)
    cosz=math.cos(rot_z)
    sinz=math.sin(rot_z)
    x_matrix = np.matrix([[1,0,0],
                          [0,cosx,-sinx],
                          [0,sinx,cosx]])
    y_matrix = np.matrix([[cosy,0,siny],
                          [0,1,0],
                          [-siny,0,cosy]])
    z_matrix = np.matrix([[cosz,-sinz,0],
                          [sinz,cosz,0],
                          [0,0,1]])
    return x_matrix*y_matrix*z_matrix

#this function is a convenience function for packing the image coordinates and focal_len into a column matrix
def imgToCam(focal_len,principle,coord_img,depth,scale=1):
    coord_cam =  np.matrix([[coord_img[0]-principle[0]],
                            [coord_img[1]-principle[1]],
                            [focal_len*scale]])
    coord_cam = coord_cam/coord_cam[2,0]*depth
    return np.concatenate((coord_cam,np.matrix([1])),0)
#this function transforms coordinates on the camera plane into vehicular coordinates
#rot_matrix from the getRotMatrix function
#cam_pos should be predetermined through measurement
#coord_cam from imgToCam
#depth is the vehicle altitude - ground altitude
#Camera Plane: principal point is 0,0, +x is left to right, +y is up to down
#Vehicle Space: +x is back to forward, +z is down to up, +y is right to left
def camToVehicle(rot_matrix,cam_pos,coord_cam):
    T = -rot_matrix.getT()*cam_pos
    RT = np.concatenate((rot_matrix,T),1)
    RT = np.concatenate((RT,np.matrix([0,0,0,1])),0)
    coord_vehicle = RT*coord_cam
    return coord_vehicle

#this function transforms coordinates from the vehicle coordinate system to lat,long,altitude
def vehicleToWorld(rot_matrix,vehicle_pos,coord_vehicle):
    T = -rot_matrix.getT()*cam_pos
    RT = np.concatenate((rot_matrix,T),1)
    RT = np.concatenate((RT,np.matrix([0,0,0,1])),0)
    coord_vehicle = np.linalg.inv(RT.getT()*RT)*RT.getT()*coord_cam
    coord_vehicle = coord_vehicle*altitude/coord_vehicle[2,0]
    return coord_vehicle

if __name__ == "__main__":
    print "Hello, World!"
