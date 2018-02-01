#include "SVMTrain.h"

int main(int argc, char *argv[])
{
	SVMTrain st("./data/train", "./data/hand_gesture_model_HOG_RBF_900.xml");
	st.get_train_data();
	st.trainAuto();
	return 0;
}