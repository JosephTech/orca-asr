
#ifndef RECORDER_H_
#define RECORDER_H_
//#include<iostream>
// #include <iostream>
#include <string>
#include <fstream>

namespace wenet{

class Recorder{
public:
    int SavePcmFile(const std::string& all_pcm_data);
private:
    
};

} // namespace wenet

#endif /* RECORDER_H_ */