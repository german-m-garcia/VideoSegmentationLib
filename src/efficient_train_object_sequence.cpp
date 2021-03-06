/*
 * main.cpp
 *
 * It reads a sequence of images and asks the user to interactively mark
 * the segments that correspond to the object class to train
 * and those that correspond to the background
 *
 *  Created on: 29 Oct 2015
 *      Author: martin
 */

/*
 * SegmentationLib
 */
#include "segmentation.h"
#include "visualisation.h"

#include <iostream>                  // for std::cout
#include <utility>                   // for std::pair
#include <algorithm>                 // for std::for_each
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graphviz.hpp>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

#include "atom.h"

#include "atom_graphs.h"
#include "GraphsMatch.h"
#include "VideoSegmentation.h"
#include "objects/ObjectDetector.h"

#include "utils.h"

using namespace boost;
using namespace videoseg;
//using namespace std;

Rect object_rect;

const string WINDOW_INTERACT = "Train Object Detector";
bool drag, drop, object_selected_ = true, select_bg_segments =false;
Mat display, tmp;

vector<Segment*> initial_bg_segments(0), bg_segments(0);
vector<Segment*> initial_fg_segments(0), fg_segments(0);
//the class selected
int segment_class = 1; //Foreground
int start_training = 0; //1 = starts training

VideoSegmentation videoSegmenter;



static void doMouseCallback(int event, int x, int y, int flags, void* param) {
	if(segment_class == 1){

		if( event == CV_EVENT_LBUTTONDOWN){
			Segment* seg=videoSegmenter.get_segment_at(y,x);

			initial_fg_segments.push_back(new Segment(*seg));

		}
	}
	else if (segment_class == 0){
		if( event == CV_EVENT_LBUTTONDOWN){
			Segment* seg=videoSegmenter.get_segment_at(y,x);
			initial_bg_segments.push_back(new Segment(*seg));
			imshow("segment",seg->getRandomColourMat());
			waitKey(0);
		}


	}


}

//the callback function for the trackbar
static void on_trackbar(int event, void* param) {
	if(segment_class == 0){
		cout <<"> click to select background segments "<<endl;
	}
	else if(segment_class == 1){
		cout <<"> drag and drop to select foreground segments "<<endl;
	}

}

void show_segments(string& text, vector<Segment*>& segs){
	Mat debugMat = Mat::zeros(tmp.rows,tmp.cols,CV_8UC3);
	for(Segment* seg: segs){
		cout <<" debug.size="<<debugMat(seg->getBoundRect()).size()<<" seg.size="<<seg->getRandomColourMat().size()<<endl;
		debugMat(seg->getBoundRect()) += seg->getRandomColourMat();
		imshow(text,debugMat);

	}
	waitKey(1);
}

void show_segments(string& text,vector<Segment>& segs){
	Mat debugMat = Mat::zeros(tmp.rows,tmp.cols,CV_8UC3);
	for(Segment& seg: segs){
		cout <<" debug.size="<<debugMat(seg.getBoundRect()).size()<<" seg.size="<<seg.getRandomColourMat().size()<<endl;
		debugMat(seg.getBoundRect()) += seg.getRandomColourMat();
		imshow(text,debugMat);

	}
	waitKey(1);
}

void user_interaction_select_object(Mat& outputMat, ObjectDetector& slc){
	display = outputMat.clone();
	tmp = display.clone();
	imshow(WINDOW_INTERACT, display);
	waitKey(0);
	if(object_selected_){

//		vector<Segment*>tmp_segments;
//		videoSegmenter.get_segments(object_rect, tmp_segments);
//		for(Segment*seg:tmp_segments)
//			initial_fg_segments.push_back(*seg);
		cout <<">output_segments.size()="<<initial_fg_segments.size()<<endl;
		string text = "segments";
		show_segments(text,initial_fg_segments);

	}
}

void propagate_segments(vector<Segment*>& initial_segments,vector<Segment*>& propagated_segments){
	for(Segment* initial_seg : initial_segments){
		Segment* propagated = videoSegmenter.get_segment_by_label(initial_seg->getLabel());
		if(propagated ==nullptr)
			cout <<"propagated segment is null, no match found for label:"<<initial_seg->getLabel()<<endl;
		else{

			propagated_segments.push_back(propagated);
		}

	}
}

int main(int argc, char** argv) {


	int scale_for_propagation = 2;
	int starting_scale = 2;
	Utils utils;
	int scales = 3;
	int gpu = 0;
	double threshold = 0.01; //0.05;
	unsigned int starting_frame = 90;
	unsigned int frame_interval = 30;
	string input_img_path, output_path;
	string svm_model_path ;
	utils.parse_args(argc, argv, threshold, scales, starting_scale,
			scale_for_propagation, gpu, input_img_path, output_path,svm_model_path);


	videoSegmenter.init(scale_for_propagation, starting_scale, scales,
			gpu, threshold);

	ObjectDetector slc(ObjectDetector::TRAIN_MODE,svm_model_path);

	//add mouse callback function for specifying the rectangular region
	cv::namedWindow(WINDOW_INTERACT,CV_WINDOW_AUTOSIZE);
	setMouseCallback(WINDOW_INTERACT, doMouseCallback, 0);

	//add trackback function for selecting either foreground or background segments
	char trackbarName[50];
	int slider_class_max = 1;
	sprintf(trackbarName, "Class (BG= 0, FG=1)= %d", slider_class_max);
	createTrackbar(trackbarName, WINDOW_INTERACT, &segment_class,
			slider_class_max, on_trackbar);
	sprintf(trackbarName, "Start training= %d", slider_class_max);
		createTrackbar(trackbarName, WINDOW_INTERACT, &start_training,
				slider_class_max, on_trackbar);

	/*
	 * open image files
	 */

	std::vector<std::string> images_list;

	std::ifstream file_colours(input_img_path);

	std::string content_colour((std::istreambuf_iterator<char>(file_colours)),
			std::istreambuf_iterator<char>());

	boost::split(images_list, content_colour, boost::is_any_of("\t \n"));

	cout << " reading input..." << argv[1] << endl;
	vector<Vec3b> colours;
	unsigned int images = starting_frame+20;//images_list.size() - 1;//

	//for(unsigned int k = starting_frame; k<frame_interval)


	for (unsigned int i = starting_frame; i < images; i++) {

		cv::Mat img, outputMat,  contours_mat, gradient, grayGradient;

		cout << images_list[i] <<  endl;

		img = cv::imread(images_list[i], -1);



		string prefix_1 = utils.get_file_name(images_list[i]);
		prefix_1 = utils.remove_extension(prefix_1);

		videoSegmenter.addImage(img, outputMat);
		imshow("video segmentation",outputMat);


		//if it is the first image
		if (i == starting_frame) {
			user_interaction_select_object(outputMat, slc);
		}
		//else need to propagate the labels of the segments
		else{

			vector<Segment*> propagated_fg_segs, propagated_bg_segs;
			//propagate the foreground segments
			propagate_segments(initial_fg_segments,propagated_fg_segs);
			//propagate the background segments
			propagate_segments(initial_bg_segments,propagated_bg_segs);
			//if the succesfully propagated segments are not the same we had in the model

			// ask the user for interaction
			if(initial_fg_segments.size() != propagated_fg_segs.size()){
				initial_fg_segments.clear();
				user_interaction_select_object(outputMat, slc);
			}

			//otherwise append the propagated segments to the fg_segments vector
			else{

				fg_segments.insert( fg_segments.end(), propagated_fg_segs.begin(), propagated_fg_segs.end() );
				bg_segments.insert( bg_segments.end(), propagated_bg_segs.begin(), propagated_bg_segs.end() );
				cout <<" fg_segments appended : fg_segments.size()="<<fg_segments.size()
						<<" bg_segments appended : bg_segments.size()="<<bg_segments.size()<<endl;

			}
			cout <<" propagated segments="<<propagated_fg_segs.size()<<endl;
			string text = "fg segments";
			show_segments(text,propagated_fg_segs);
			text = "bg segments";
			show_segments(text,propagated_bg_segs);
			slc.add_training_data(propagated_fg_segs,propagated_bg_segs);

		}
		if(start_training)
		{
			slc.train();
			return 0;
		}



	}
	//slc.add_training_data(fg_segments,bg_segments);
	slc.train();

//
//	vector<cv::Mat> outputPyr = segmentation.getOutputSegmentsPyramid();
	return 0;
}

