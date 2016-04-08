import cv2
import numpy as np

def camToVehicle(calib_matrix,rot_matrix,cam_pos,coord_cam,altitude)
{
    #C = -R*T
    #T = -Rt*T
    T = -rot_matrix.getT()*cam_pos
    KRT = calib_matrix * rot_matrix * T
    coord_vehicle = np.linalg.inv(KRT.getT()*KRT)*KRT.getT()*coord_cam
    coord_vehicle = coord_vehicle*altitude/coord_vehicle[2,0]
    return coord_vehicle
}

def vehicleToWorld(rot_matrix,vehicle_pos,coord_vehicle)
{
    T = -rot_matrix.getT()*cam_pos
    KRT = calib_matrix * rot_matrix * T
    coord_vehicle = np.linalg.inv(KRT.getT()*KRT)*KRT.getT()*coord_cam
    coord_vehicle = coord_vehicle*altitude/coord_vehicle[2,0]
    return coord_vehicle
}

if __name__ == "__main__":
    print "Hello, World!"
