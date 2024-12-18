#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include <iostream>
#include <fstream>
#include <vector>

int main()
{
    system("title ����python");//����cmd���ڱ���
    //system("mode con cols=150 lines=40");//���ڿ�ȸ߶�
    system("color 0B");
    system("date /T");
    system("TIME /T");
    //system("ubuntu2004");//��ubuntu
 
    /////////////////////////////////////////////////////////////////////////////�޸Ĳ���
    // ʹ�� wsl �� -e bash -c ִ�ж������
    const char* commands =
    "wsl ~ -e bash -c \""
    //"ls; "
    "cd /mnt/c/Users/USTC/Desktop/yyh/CAPRI-Net-Vertion-C;"//����python�ļ�����Ŀ¼
    "source ~/anaconda3/etc/profile.d/conda.sh;"//�����
    "conda activate th22;"//�����
    "python voxel.py;"//ִ��voxel
    "python fine-tuning.py -e lll -g 0 --test --voxel --start 0 --end 1;"//ִ��fine tuning
    "python test.py -e lll -g 0 -p 2 -c best_stage2 --test --voxel --start 0 --end 1 --csg;"//ִ��test
    "python test.py -e lll -g 0 -p 2 -c best_stage2 --test --voxel --start 0 --end 1;"//ִ��test
    "\"";  // ע������û�ж���ķֺ�

    int result = system(commands);

    if (result == -1) {
        printf("Failed to execute commands in Ubuntu.\n");
    }
    else {
        printf("Commands executed successfully in Ubuntu.\n");
    }
    /////////////////////////////////////////////////////////////////////////////�޸Ĳ���







    /////////////////////////////////////////////////////////////////////////////////�·������������




    // �򿪶������ļ�
    std::ifstream file("./csg_tree.bin", std::ios::binary);

    if (!file.is_open()) {
        std::cerr << "Failed to open file." << std::endl;
        return EXIT_FAILURE;
    }

    // ��ȡ�ļ���С���Ա�ȷ���ж��ٸ�Ԫ��
    file.seekg(0, file.end);
    size_t num_elements = file.tellg() / sizeof(float);
    file.seekg(0, file.beg);

    // ��������������
    const int cols = 8; // ÿ���� 8 �� float
    size_t rows = num_elements / cols;

    if (num_elements % cols != 0) {
        std::cerr << "File size does not match expected number of elements." << std::endl;
        return EXIT_FAILURE;
    }

    // ����һ���������洢����
    std::vector<float> data(num_elements);

    // ��ȡ����
    file.read(reinterpret_cast<char*>(data.data()), num_elements * sizeof(float));

    if (!file) {
        std::cerr << "Error: only " << file.gcount() << " could be read" << std::endl;
        return EXIT_FAILURE;
    }

    file.close();

    std::vector<std::vector<float>> data_2D(rows, std::vector<float>(cols));

    // ������ת��Ϊ��ά�ṹ�������ӡ�ͺ�������
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


