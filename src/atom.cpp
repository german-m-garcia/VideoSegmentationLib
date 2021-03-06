/*
 * atom.cpp
 *
 *  Created on: 29 Oct 2015
 *      Author: martin
 */

#include "atom.h"

namespace videoseg {

int Atom::static_id = 0;

Atom::Atom() :
		segment_(nullptr), id(static_id++),id_matched_to(-1) {
	// TODO Auto-generated constructor stub

}

Atom::Atom(Segment* segment) :
		segment_(segment), id(static_id++),id_matched_to(-1) {

}

Atom::Atom(const Atom& other):
		segment_(other.segment_),id(other.id),id_matched_to(other.id_matched_to){

}



Atom::~Atom() {
	//cout <<"deleting Atom"<<endl;
	// TODO Auto-generated destructor stub
}

const Point2i& Atom::getCenter() const{
	return segment_->getCenter();
}


double Atom::similarity(const Atom* other) const{


	//check whether the two segments are too far away

	Point2i distance_centers = segment_->getCenter() - other->segment_->getCenter();
	double dist = sqrt(distance_centers.x*distance_centers.x + distance_centers.y*distance_centers.y);
	if(dist > MAX_ALLOWED_DISTANCE)
		return 0;


	double score_h = .5*(compareHist(segment_->getHHist(),other->segment_->getHHist(), CV_COMP_CORREL)+1);
	double score_s = .5*(compareHist(segment_->getSHist(),other->segment_->getSHist(), CV_COMP_CORREL)+1);
	double score_v = .5*(compareHist(segment_->getVHist(),other->segment_->getVHist(), CV_COMP_CORREL)+1);

//	cout << "scores for HSV= " << score_h << "," << score_s << "," << score_v
//			<< "," << endl;
//	cout << "HSV score=" << (score_h + score_s + score_v) << endl;

	double module_1 =
			segment_->getEigenVal()[0] > other->segment_->getEigenVal()[0] ?
					other->segment_->getEigenVal()[0]
							/ segment_->getEigenVal()[0] :
					segment_->getEigenVal()[0]
							/ other->segment_->getEigenVal()[0];

	double angle_1 = segment_->getOrientations()[0] > other->segment_->getOrientations()[0] ? other->segment_->getOrientations()[0]/segment_->getOrientations()[0] : segment_->getOrientations()[0]/other->segment_->getOrientations()[0];


	double module_2 =
			segment_->getEigenVal()[1] > other->segment_->getEigenVal()[1] ?
					other->segment_->getEigenVal()[1]
							/ segment_->getEigenVal()[1] :
					segment_->getEigenVal()[1]
							/ other->segment_->getEigenVal()[1];

//	cout <<" segment angle="<<segment_->getOrientations()[1]<<endl;
//	cout <<" other->segment angle="<<other->segment_->getOrientations()[1]<<endl;
	double angle_2 = segment_->getOrientations()[1] > other->segment_->getOrientations()[1] ? other->segment_->getOrientations()[1]/segment_->getOrientations()[1] : segment_->getOrientations()[1]/other->segment_->getOrientations()[1];


//	cout << "PCA_1=" << module_1 << " angle_1= "<<angle_1<< endl;
//	cout << "PCA_2=" << module_2 << " angle_2= "<<angle_2<< endl;
	double score_pca = (module_1 * angle_1 + module_2*angle_2)*0.5;
//	cout << "score_pca=" << score_pca << endl;
	double score_hsv = score_h*score_s*score_v;

	//return (score_h+score_s+score_v);
	return (score_pca+score_hsv)*0.5;

}

} /* namespace imgseg */
