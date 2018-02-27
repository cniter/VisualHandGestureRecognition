// --------- Name ---------
提取：extract 
检测：detect(Dtc)
识别：recognize
分割：segment

// ---------- Code Style -----------------
类名：全大写 eg:class HandDetector

方法名：首字母小写 eg:cv::Mat drawHand()

变量名：小写+下划线 eg:float max_distance

私有变量：小写+下划线 eg:int color_

函数参数：输入参数，输出参数 eg:void drawHand(Mat input_mat, Mat output_mat)

行内注释：注释符号// 和代码间以制表符Tab键隔开，注释符号// 后空一格写注释，eg:double max_distance;	// 最大距离

行间注释：注释符号// 写在待注释代码上方，并与其平齐，eg:
// 手势检测器
class HandDetector

大段注释：/*********************************************
	  * xxxxx
	  * xxxxx
	  *********************************************/