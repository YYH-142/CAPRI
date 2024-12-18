#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include <iostream>
#include <fstream>
#include <vector>

int main()
{
    system("title 调用python");//设置cmd窗口标题
    //system("mode con cols=150 lines=40");//窗口宽度高度
    system("color 0B");
    system("date /T");
    system("TIME /T");
    //system("ubuntu2004");//打开ubuntu
 
    /////////////////////////////////////////////////////////////////////////////修改部分
    // 使用 wsl 和 -e bash -c 执行多个命令
    const char* commands =
    "wsl ~ -e bash -c \""
    //"ls; "
    "cd /mnt/c/Users/USTC/Desktop/yyh/CAPRI-Net-Vertion-C;"//到达python文件所在目录
    "source ~/anaconda3/etc/profile.d/conda.sh;"//激活环境
    "conda activate th22;"//激活环境
    "python voxel.py;"//执行voxel
    "python fine-tuning.py -e lll -g 0 --test --voxel --start 0 --end 1;"//执行fine tuning
    "python test.py -e lll -g 0 -p 2 -c best_stage2 --test --voxel --start 0 --end 1 --csg;"//执行test
    "python test.py -e lll -g 0 -p 2 -c best_stage2 --test --voxel --start 0 --end 1;"//执行test
    "\"";  // 注意这里没有多余的分号

    int result = system(commands);

    if (result == -1) {
        printf("Failed to execute commands in Ubuntu.\n");
    }
    else {
        printf("Commands executed successfully in Ubuntu.\n");
    }
    /////////////////////////////////////////////////////////////////////////////修改部分







    /////////////////////////////////////////////////////////////////////////////////下方代码谨慎操作




    // 打开二进制文件
    std::ifstream file("./csg_tree.bin", std::ios::binary);

    if (!file.is_open()) {
        std::cerr << "Failed to open file." << std::endl;
        return EXIT_FAILURE;
    }

    // 获取文件大小，以便确定有多少个元素
    file.seekg(0, file.end);
    size_t num_elements = file.tellg() / sizeof(float);
    file.seekg(0, file.beg);

    // 计算行数和列数
    const int cols = 8; // 每行有 8 个 float
    size_t rows = num_elements / cols;

    if (num_elements % cols != 0) {
        std::cerr << "File size does not match expected number of elements." << std::endl;
        return EXIT_FAILURE;
    }

    // 创建一个向量来存储数据
    std::vector<float> data(num_elements);

    // 读取数据
    file.read(reinterpret_cast<char*>(data.data()), num_elements * sizeof(float));

    if (!file) {
        std::cerr << "Error: only " << file.gcount() << " could be read" << std::endl;
        return EXIT_FAILURE;
    }

    file.close();

    std::vector<std::vector<float>> data_2D(rows, std::vector<float>(cols));

    // 将数据转换为二维结构，方便打印和后续处理
    for (size_t i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            data_2D[i][j] = data[i * cols + j];
            std::cout << data_2D[i][j] << " ";
        }
        std::cout << std::endl;
    }

    std::cout << "CSG Tree loaded from csg_tree.bin successfully." << std::endl;

    return EXIT_SUCCESS;


  
    //system("pause");
    //exit(0);
}


