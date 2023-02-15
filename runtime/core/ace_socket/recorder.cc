#include "ace_socket/recorder.h"

namespace wenet{

int Recorder::SavePcmFile(const std::string& all_pcm_data)
{    
    printf("Recorder::SavePcmFile()被调用..\n");
    std::ofstream file;
    file.open("./test.pcm", std::ios::binary | std::ios::out);
    file << all_pcm_data;

    file.close();
    return 0;

    return 0;
}

} // namespace wenet
