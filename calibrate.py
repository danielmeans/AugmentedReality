import numpy as np
import cv2 as cv
import glob


def calibrate():
    image_directories = ['checkerboardFromVideo/*.jpg', 'checkerboardPhotos/*.jpg']

    for dir in image_directories:
        # termination criteria
        criteria = (cv.TERM_CRITERIA_EPS + cv.TERM_CRITERIA_MAX_ITER, 30, 0.001)
        # prepare object points, like (0,0,0), (1,0,0), (2,0,0) ....,(6,5,0)
        objp = np.zeros((6*8,3), np.float32)
        objp[:,:2] = np.mgrid[0:8,0:6].T.reshape(-1,2)
        # Arrays to store object points and image points from all the images.
        objpoints = [] # 3d point in real world space
        imgpoints = [] # 2d points in image plane.
        images = glob.glob(dir)
        for fname in images:
            img = cv.imread(fname)
            gray = cv.cvtColor(img, cv.COLOR_BGR2GRAY)
            # Find the chess board corners
            ret, corners = cv.findChessboardCorners(gray, (8,6), None)
            # If found, add object points, image points (after refining them)
            if ret == True:
                objpoints.append(objp)
                corners2 = cv.cornerSubPix(gray,corners, (11,11), (-1,-1), criteria)
                imgpoints.append(corners)
                # Draw and display the corners
                cv.drawChessboardCorners(img, (8,6), corners2, ret)
                cv.imshow('img', img)
                cv.waitKey(500)
        cv.destroyAllWindows()

        ret, mtx, dist, rvecs, tvecs = cv.calibrateCamera(objpoints, imgpoints, gray.shape[::-1], None, None)
        # img = cv.imread('checkerboardFromVideo/frame0054.jpg')
        # h,  w = img.shape[:2]
        # newcameramtx, roi = cv.getOptimalNewCameraMatrix(mtx, dist, (w,h), 1, (w,h))
        # # undistort
        # dst = cv.undistort(img, mtx, dist, None, newcameramtx)
        # # crop the image
        # x, y, w, h = roi
        # dst = dst[y:y+h, x:x+w]
        # cv.imwrite('calibresult.png', dst)
        h,  w = img.shape[:2]
        horiz_field_of_view = 2 * np.arctan(w / (2 * mtx[0][0]))
        vert_field_of_view = 2 * np.arctan(h / (2 * mtx[1][1]))


        #store parameters to be saved into an array
        parameter_array = ["Horizontal FoV: " + str(horiz_field_of_view) + '\n']
        parameter_array.append("Vertical FoV: " + str(vert_field_of_view) + '\n')
        parameter_array.append('fx: ' + str(mtx[0][0]) + '\n')
        parameter_array.append('fy: ' + str(mtx[1][1]) + '\n')
        parameter_array.append('cx: ' + str(mtx[0][2]) + '\n')
        parameter_array.append('cy: ' + str(mtx[1][2]) + '\n')
        parameter_array.append('k1: ' + str(dist[0][0]) + '\n')
        parameter_array.append('k2: ' + str(dist[0][1]) + '\n')
        parameter_array.append('k3: ' + str(dist[0][2]) + '\n')
        parameter_array.append('p1: ' + str(dist[0][3]) + '\n')
        parameter_array.append('p2: ' + str(dist[0][4]) + '\n')



        #write parameters to a file
        with open('README.txt', 'a') as file:
            for param in parameter_array:
                file.write(param)

        mean_error = 0
        total_error = 0
        for i in range(len(objpoints)):
            imgpoints2, _ = cv.projectPoints(objpoints[i], rvecs[i], tvecs[i], mtx, dist)
            error = cv.norm(imgpoints[i], imgpoints2, cv.NORM_L2)
            mean_error += error/len(imgpoints2)
            total_error += error
        print( "mean error: {}".format(mean_error/len(objpoints)) )
        print( "total error: {}".format(total_error/len(objpoints)) )
        return mtx, rvecs
