#include "ps3_oreimo.h"

struct Header
{
    uint64_t magic;
    uint64_t filesize;
    uint32_t fileNULL1;
    uint16_t filenumber;
    uint16_t fileNULL2;
};

struct INFO
{
    uint64_t filedata_beg;
    uint64_t fileNULL;
    uint64_t offset;
    uint64_t filename_offset;
};

int main(int argc, char* argv[]){
    //获取原文件头部信息
    if(!argv[1]){
        cerr << "文件打开失败" << endl;
        return -1;
    }
    ifstream file(argv[1], ios::binary);
    string new_filename = argv[1];
    Header hd;
    INFO fo;
    file.seekg(0, ios::beg);
    file.read(reinterpret_cast<char*>(&hd),sizeof(Header));

    ofstream newfile(new_filename + "new", ios::binary);
    newfile.write(reinterpret_cast<char*>(&hd),sizeof(Header));

    size_t index = static_cast<int>(hd.filenumber);

    vector<uint64_t> data_beg(index);
    vector<uint64_t> null(index);
    vector<uint64_t> data_offset(index);
    vector<uint64_t> name_offset(index);

    //写入原文件信息
    for(size_t i = 0; i < index; i++){
        file.read(reinterpret_cast<char*>(&fo),sizeof(INFO));
        data_beg[i] = fo.filedata_beg;
        null[i] = fo.fileNULL;
        data_offset[i] = fo.offset;
        name_offset[i] = fo.filename_offset;
        newfile.write(reinterpret_cast<char*>(&data_beg[i]),sizeof(data_beg[i]));
        newfile.write(reinterpret_cast<char*>(&null[i]), sizeof(null[i]));
        newfile.write(reinterpret_cast<char*>(&data_offset[i]), sizeof(data_offset[i]));
        newfile.write(reinterpret_cast<char*>(&name_offset[i]), sizeof(name_offset[i]));   
    }

    vector<uint32_t> filename_off(index);
    vector<string> filename(index);

    //写入文件名
    for (size_t i = 0; i < index; i++){
        file.seekg(static_cast<streampos>(name_offset[i]), ios::beg);
        file.read(reinterpret_cast<char*>(&filename_off[i]), 4);
        filename[i].resize(filename_off[i]);
        file.read(filename[i].data(), filename[i].size());
        newfile.write(reinterpret_cast<char*>(&filename_off[i]), sizeof(filename_off[i]));
        newfile.write(filename[i].data(), filename[i].size());
    }
    
    //写入数据
    size_t size = static_cast<uint64_t>(data_beg[0]) - file.tellg();
    cout << size << endl;
    vector<uint8_t> data(size);
    file.read(reinterpret_cast<char*>(data.data()), data.size());
    newfile.write(reinterpret_cast<char*>(data.data()), data.size());
    file.close();

    //读取并写入文件
    for(size_t i = 0; i < index; i++){
        ifstream file(filename[i], ios::binary);
        file.seekg(0, ios::end);
        size_t size = file.tellg();
        file.seekg(0, ios::beg);
        vector<uint8_t> filedata(size);
        file.read(reinterpret_cast<char*>(filedata.data()), filedata.size());
        cout << filedata.size() << endl;
        newfile.seekp(static_cast<streamoff>(data_beg[i]), ios::beg);
        newfile.write(reinterpret_cast<char*>(filedata.data()),filedata.size());
    }

}