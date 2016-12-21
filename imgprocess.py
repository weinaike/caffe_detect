#!/usr/bin/env python
#-*- coding: utf-8 -*-

__author__ = 'zstu'

import cv2
import os
import numpy as np
import sys
import math
from multiprocessing import Process
import time
import random

reload(sys)
sys.setdefaultencoding('utf8')

resizew = 256
resizeh = 256

#判断是否是.jpg或.JPG格式图像
def isJPG(img):
	if os.path.splitext(img)[1] == '.jpg' or os.path.splitext(img)[1] == '.JPG':
		return True #是
	else:
		return False
#end of isJPG

def rotate_about_center(src, angle, scale=1.): #旋转图片
	w = src.shape[1]
	h = src.shape[0]
	rangle = np.deg2rad(angle)  # angle in radians
	# now calculate new image width and height
	nw = (abs(np.sin(rangle)*h) + abs(np.cos(rangle)*w))*scale
	nh = (abs(np.cos(rangle)*h) + abs(np.sin(rangle)*w))*scale
	# ask OpenCV for the rotation matrix
	rot_mat = cv2.getRotationMatrix2D((nw*0.5, nh*0.5), angle, scale)
	# calculate the move from the old center to the new center combined
	# with the rotation
	rot_move = np.dot(rot_mat, np.array([(nw-w)*0.5, (nh-h)*0.5,0]))
	# the move only affects the translation, so update the translation
	# part of the transform
	rot_mat[0,2] += rot_move[0]
	rot_mat[1,2] += rot_move[1]
	return cv2.warpAffine(src, rot_mat, (int(math.ceil(nw)), int(math.ceil(nh))), flags=cv2.INTER_LANCZOS4)

#此函数共旋转图像6次 
def rotateImage(img, dstPath, clas, num): #参数为待分割图像，目的路径，所属类别，以及编号
	for dig in range(0, 91, 90): #旋转角度为(0, 90)
		for deltad in range(-15, 16, 15): #旋转角度为(-15, 0, 15)
			imgrot = rotate_about_center(img, dig+deltad)
			
			#cut the center image
			width = imgrot.shape[0]
			height = imgrot.shape[1]
			if width > height:
				left = width/2 - height*1/3
				top = height/6
				right = left+height*2/3
				bottom = top+height*2/3
			else:
				left = width/6
				top = height/2-width*1/3
				right = left+width*2/3
				bottom = top+width*2/3
			imgroi = imgrot[left:right, top:bottom]
			
			#resize the image to resizew x resizeh
			res = cv2.resize(imgroi, (resizew, resizeh), interpolation=cv2.INTER_CUBIC)
			
			#save the image
			cv2.imwrite(dstPath+"/"+clas+"_"+str(num)+".jpg", res)
			num = num+1
#end of rotateImage

#根据给定的length从图像中心截取正方形图片
def scaled_image(img, length):
	height,width = img.shape[:2] #获取图像的高，宽
	left = (width - length) / 2
	top = (height - length) / 2
	right = left + length
	bottom = top + length
	scaledImg = img[top:bottom, left:right]
	return scaledImg
#end of scaled_image

#此函数截取图片中心正方形，截取长度为原始图片最短边长的（0.9,0.8,0.7）倍，然后缩放到resizew x resizeh
def scaledImage(img, dstPath, clas, num): #参数为待缩放图像，目的路径，所属类别，以及编号
	height,width = img.shape[:2]
	if height > width: #取height和width中小的值
		length = width
	else:
		length = height
	paramList = [1.0, 0.95, 0.9]
	#crop the image	
	for param in paramList:
		scaledImg = scaled_image(img, int(length*param))
		#resize
		scaledImg_resize = cv2.resize(scaledImg, (resizew, resizeh), interpolation = cv2.INTER_CUBIC)
		#save image		
		cv2.imwrite(dstPath+"/"+clas+"_"+str(num)+".jpg", scaledImg_resize)
		num = num+1
#end of scaledImage

#此函数仅截取图片中心正方形，截取长度为原始图片最短边长
def cropImage(img, dstPath, clas, num): #参数为待缩放图像，目的路径，所属类别，以及编号
	height,width = img.shape[:2]
	if height > width:
		length = width
	else:
		length = height
	#crop the image
	left = (width - length) / 2
	top = (height - length) / 2
	right = left + length
	bottom = top + length
	cropImg = img[top:bottom, left:right]
	#resize
	cropImg_resize = cv2.resize(cropImg, (resizew, resizeh), interpolation=cv2.INTER_CUBIC)
	#save image
	cv2.imwrite(dstPath+"/"+clas+"_"+str(num)+".jpg", cropImg_resize)
#end of cropImage	

#此函数用于调整图像亮度，取图像img亮度的80%,90%,110%,120%共4个
def change_luminance(img, dstPath, clas, num):
	brightlist = [9.5, 10,10.5]
	for bright in brightlist:
		luminImg = cv2.addWeighted(img, bright*0.1, img, 0.0, 0)
		#resize image
		luminImg_resize = cv2.resize(luminImg, (resizew, resizeh), interpolation = cv2.INTER_CUBIC)
		#save image
		cv2.imwrite(dstPath+"/"+clas+"_"+str(num)+".jpg", luminImg_resize)
		num = num+1
#end of change_luminance

#生成用于val部分的图像，取图像亮度的80%,120%共2个
def change_luminance_val(img, dstPath, clas, num):
	brightlist = [10]
	for bright in brightlist:
		luminImg = cv2.addWeighted(img, bright*0.1, img, 0.0, 0)
		#resize image
		luminImg_resize = cv2.resize(luminImg, (resizew, resizeh), interpolation = cv2.INTER_CUBIC)
		#save image
		cv2.imwrite(dstPath+"/"+clas+"_"+str(num)+".jpg", luminImg_resize)
		num = num+1
#end of change_luminance_val
	
#剪切输入的原图，分别为按宽取80%(2张)，按长取80%(2张)共4张
def crop_sides(img, dstPath, clas, num):
	height,width = img.shape[:2]
	leng_hei = int(height * 0.8) #按长剪切的值
	leng_wid = int(width * 0.8) #按宽剪切的值
	#先按宽剪切
	left1 = 0
	right1 = leng_wid
	img_1 = img[0:height, left1:right1]
	cropImage(img_1, dstPath, clas, num)
	num = num + 1

	left2 = width - leng_wid
	right2 = width
	img_2 = img[0:height, left2:right2]
	cropImage(img_2, dstPath, clas, num)
	num = num + 1
	#按长剪切
	top1 = 0
	bottom1 = leng_hei
	img_3 = img[top1:bottom1, 0:width]
	cropImage(img_3, dstPath, clas, num)
	num = num + 1

	top2 = height - leng_hei
	bottom2 = height
	img_4 = img[top2:bottom2, 0:width]
	cropImage(img_4, dstPath, clas, num)
	num = num + 1
#end of crop_sides

#统计src_dir目录下.jpg或.JPG格式图像数目
def count_jpg(src_dir):
	count = 0
	filelist = os.listdir(src_dir)
	for filename in filelist:
		if isJPG(filename): 
			#是
			count = count + 1
		#end of if
	#end of for
	return count #返回图像数目
#end of count_jpg

NNUM =30  #每一类图像要处理的val类别的数目

#在输入路径par_path下分割输入列表catelist中的图片,结果存放在dst_path路径下
def crops(catelist, parent_path, dst_path):
	print '此子进程(%s)要分割的类别数目为%d'%(os.getpid(), len(catelist)) #打印
	start = time.time()
	dst_path = os.path.join(dst_path, str(os.getpid()))
	if not os.path.exists(dst_path):
		os.mkdir(dst_path)
	train_dir = os.path.join(dst_path, "training") #training部分存储路径
	val_dir = os.path.join(dst_path, "val") #val部分存储路径
	if not os.path.exists(train_dir): #若不存在training路径，则新建
		os.mkdir(train_dir)
	if not os.path.exists(val_dir):
		os.mkdir(val_dir)
	num_filelist = 0 #处理类别数目
	print '子进程(%s)开始分割'%(os.getpid())
	for cate in catelist:
		#生成存放结果的文件夹
		cate_train = os.path.join(train_dir, cate)
		if not os.path.exists(cate_train): #判断training结果路径中是否存在存放当前类的文件夹
			os.mkdir(cate_train)
		cate_val = os.path.join(val_dir, cate) 
		if not os.path.exists(cate_val): #判断val结果路径是否存在当前类的文件夹
			os.mkdir(cate_val)
		cate_dir = os.path.join(parent_path, cate) #生成要处理图像类别的绝对路径
		if not os.path.isdir(cate_dir):
			print cate_dir,'is not a dir'
			continue
		filelist = os.listdir(cate_dir)
		leng = count_jpg(cate_dir) #统计该类图片数目
		NN = leng / NNUM #从总的图片数目中分出20张用作测试图片
		count = 0 #统计生成图片数目
		num = 0 #统计处理图片数目
		val_num = 0
 		for filename in filelist: #遍历每一类所有的图片
			if os.path.splitext(filename)[1] == '.jpg' or os.path.splitext(filename)[1] == '.JPG':
				#如果该文件名为jpg或JPG格式图像，则读取
				img = cv2.imread(os.path.join(cate_dir, filename))
			        height,width = img.shape[:2]
   				if height > width:
            			    length = width
       				else:
     				    length = height
     				#crop the image
  				left = (width - length) / 2
       				top = (height - length) / 2
   			        right = left + length
      				bottom = top + length
			        imgorg = img[top:bottom, left:right]
			else:
				continue
			if imgorg == None: #读取不成功
				print os.path.join(cate_dir, filename),"is None"
				continue
			#每一类图片每隔NN张用作val，其他用作training 
			if num % NN == 0 and val_num < NNUM:
				val_num = val_num + 1
				dst_dir = cate_val
				#根据亮度分割图像
				change_luminance(imgorg, dst_dir, cate, count)
				count = count + 4
				#根据缩放比例分割图像
				#scaledImage(imgorg, dst_dir, cate, count)
				#count = count + 3
				#按边长分割图像
				# crop_sides(imgorg, dst_dir, cate, count)
				# count = count + 4
				#旋转图像
				#rotateImage(imgorg, dst_dir, cate, count)
				#count = count + 6
				#左右翻转图像
				#imgflip = cv2.flip(imgorg, 1)
				#旋转图像
				#rotateImage(imgflip, dst_dir, cate, count)
				#count = count + 6
			else: #用作val功能的图像
				#用作training功能的图像一张分割为23张
				dst_dir = cate_train
				#根据亮度分割图像
				change_luminance(imgorg, dst_dir, cate, count)
				count = count + 4
				#根据缩放比例分割图像
				#scaledImage(imgorg, dst_dir, cate, count)
				#count = count + 3
				#按边长分割图像
				# crop_sides(imgorg, dst_dir, cate, count)
				# count = count + 4
				#旋转图像
				#rotateImage(imgorg, dst_dir, cate, count)
				#count = count + 6
				#左右翻转图像
				#imgflip = cv2.flip(imgorg, 1)
				#旋转图像
				#rotateImage(imgflip, dst_dir, cate, count)
				#count = count + 6
			num = num + 1
		num_filelist = num_filelist + 1
		if num_filelist % 3 == 0:
			print '**************************子进程(%s)已处理%d类图片'%(os.getpid(), num_filelist)
		print cate,count,num,leng #输出类别，生成图片数，处理图片数，总共文件数目
	end = time.time()
	print 'Task %s runs %0.2f seconds.' % (os.getpid(), (end - start)) 
#end of crops

if __name__ == '__main__':
	#从文件中读取要分割的类别
	fi = open("filelist.txt", 'r')
	catelist = []
	for line in fi.readlines():
		# if int(line.strip().split()[1]) > 50: #取图片数大于100的图片进行分割
		catelist.append(line.strip().split()[0])
	leng = len(catelist)
	print "总的类别数目为%d"%leng #输出
	#将类别均等分为12份
	N = 3
	cate_list = []
	for i in range(N):
		cate_list.append([])
	for i in range(leng):
		cate_list[i % N].append(catelist[i])
	'''
	for i in range(N):
		for j in range(len(cate_list[i])):
			print cate_list[i][j]
		print "************8"	
	'''
	#进行多进程操作
	parent_path = "/home/joyoung/digits/data/show2016_dealed/new"
	dst_path = os.path.join(os.getcwd())
	'''
	p = Pool()
	for i in range(8):
		p.apply_async(crops, args=(cate_list[i], parent_path, dst_path,))
	#end of for
	print 'Waiting for all subprocesses done...'
	p.close()
	p.join()
	print 'All subprocesses done.'
	'''
	for i in range(N):
		p = Process(target=crops, args=(cate_list[i], parent_path, dst_path,))
		print '%d进程开始'%(i)
		p.start()
                #crops(cate_list[i],parent_path,dst_path)
		#p.join()
		print '%d进程结束'%(i)
	print '结束'
#end
