#!/usr/bin/python
import numpy as np
import math
import TargetLoc as TL

def main():
    #camera intrinsics
    focal_len = 3.6 #in mm
    scale = 1/1.4e-3 #px per mm
    img_width = 2592 #in px
    img_height = 1944 #in px
    principle = [img_width/2,img_height/2]
    #params for the test
    depth = 50 #meters???
    #coord_img = [img_width/2,img_height/2]
    coord_img = [img_width/2,0]
    rot_x = math.pi
    rot_y = 0
    rot_z = math.pi/2
    cam_pos = np.matrix([[0],[0],[-1]])
    #begin the test
    coord_cam = TL.imgToCam(focal_len,principle,coord_img,depth,scale)
    print "coord_cam:"
    print coord_cam
    rot_matrix = TL.getRotMatrix(rot_x,rot_y,rot_z)
    print "rot_matrix:"
    print rot_matrix
    coord_vehicle = TL.camToVehicle(rot_matrix,cam_pos,coord_cam)
    coord_vehicle = coord_vehicle/coord_vehicle[3,0]
    print "Projected [%f,%f] to [%f,%f,%f]"%(coord_img[0],
                                             coord_img[1],
                                             coord_vehicle[0,0],
                                             coord_vehicle[1,0],
                                             coord_vehicle[2,0])
if __name__ == "__main__":
    main()
