#include "ps3_oreimo.h"

struct Header
{
    uint64_t magic;
    uint64_t filesize;
    uint32_t fileNULL1;
    uint16_t filenumber;
    uint16_t fileNULL3;
};

struct INFO
{
    uint64_t filedata_beg;
    uint64_t fileNULL;
    uint64_t offset;
    uint64_t filename_offset;
};

int main(int argc, char** argv){
    for(int i = 1; i < argc; i++){
        if (!argv[i]) {
            cerr << "文件名参数为空" << endl;
            return 1;
        }

        path pathObj(argv[i]);

        if (!exists(pathObj)) {
            cerr << "文件不存在: " << pathObj << endl;
            return 1;
        }

        ifstream file(pathObj, ios::binary);
        
        string foldername_no_ext = pathObj.stem().string();
        create_directories(foldername_no_ext);

        Header hd;
        INFO io;

        file.seekg(0, ios::beg);

        vector<uint8_t> DATA_BLOCK;

        file.read(reinterpret_cast<char*>(&hd), sizeof(Header));

        size_t index = static_cast<int>(hd.filenumber);

        vector<uint64_t> data_beg(index);
        vector<uint64_t> data_offset(index);
        vector<uint64_t> name_offset(index);

        file.seekg(sizeof(Header), ios::beg);

        for(size_t i = 0; i < index; i++){
            if(!file.read(reinterpret_cast<char*>(&io), sizeof(INFO))){
                cerr << "文件重新读取错误";
            }
            data_beg[i] = io.filedata_beg;
            data_offset[i] = io.offset;
            name_offset[i] = io.filename_offset;
        }

        vector<uint32_t> filename_size(index);
        vector<string> filename(index);
        
        for(size_t i = 0; i < index; i++){
            file.seekg(static_cast<streamoff>(name_offset[i]), ios::beg);
            file.read(reinterpret_cast<char*>(&filename_size[i]), 4);
            filename[i].resize(filename_size[i]);
            file.read(filename[i].data(), filename_size[i]);
        }   

        for(size_t i = 0; i < index; i++){
            cout << filename[i].data()  << endl;
        }

        for(size_t i = 0; i < index; i++){
            path filepath = path(foldername_no_ext) / filename[i];
            if(i == (index-1)){
                file.seekg(static_cast<streamoff>(data_beg[i]), ios::beg);
                file.seekg(0, ios::end);
                size_t file_size = file.tellg();
                size_t size_to_read = file_size - static_cast<streamoff>(data_beg[i]);
                file.seekg(static_cast<streamoff>(data_beg[i]), ios::beg);
                vector<uint8_t> filedata(size_to_read);
                file.read(reinterpret_cast<char*>(filedata.data()), static_cast<streamsize>(size_to_read));
                ofstream createfile(filepath, ios::binary);
                createfile.write(reinterpret_cast<char*>(filedata.data()), static_cast<streamsize>(size_to_read));                
            }else{
                size_t size = data_beg[i+1] - data_beg[i];
                vector<uint8_t> filedata(size);
                file.seekg(static_cast<streamoff>(data_beg[i]), ios::beg);
                file.read(reinterpret_cast<char*>(filedata.data()), static_cast<streamsize>(size));
                ofstream createfile(filepath, ios::binary);
                createfile.write(reinterpret_cast<char*>(filedata.data()), static_cast<streamsize>(size));        
            }

        }
    }

}