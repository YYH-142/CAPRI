import trimesh
import numpy as np
import h5py
from itertools import product
import trimesh.voxel as voxel
import trimesh.voxel.ops as voxel_ops

# 读取 .obj 文件
mesh = trimesh.load('./input/fandisk.obj')

# 计算原始网格的边长
original_max_extent = mesh.extents.max()

# 获取顶点坐标
vertices = mesh.vertices

# 分别计算 x、y、z 方向的最大值和最小值
# x_min, y_min, z_min = vertices.min(axis=0)
# x_max, y_max, z_max = vertices.max(axis=0)

# vertices[:, 0] -= x_min
# vertices[:, 1] -= y_min
# vertices[:, 2] -= z_min
vertices = vertices/original_max_extent/1.08

# 更新网格的顶点
mesh.vertices = vertices

# 输出更新后的顶点坐标
# print("Updated vertices:")
# print(vertices)

# 如果需要，可以输出新的 x、y、z 方向的最大值和最小值
# x_min_new, y_min_new, z_min_new = vertices.min(axis=0)
# x_max_new, y_max_new, z_max_new = vertices.max(axis=0)

# print("New X min:", x_min_new)
# print("New X max:", x_max_new)
# print("New Y min:", y_min_new)
# print("New Y max:", y_max_new)
# print("New Z min:", z_min_new)
# print("New Z max:", z_max_new)

target_resolution = 64

# 体素化三角网格
voxel_size = 1.0 / target_resolution  # 体素大小为 1/64
voxelized_mesh = mesh.voxelized(pitch=voxel_size).fill()

# 获取体素矩阵
voxel_matrix = voxelized_mesh.matrix

###################################################################################################
# 将体素矩阵转换为VoxelGrid对象
voxel_grid = trimesh.voxel.VoxelGrid(voxel_matrix)
# 从VoxelGrid对象中提取网格
mesh = voxel_ops.matrix_to_marching_cubes(matrix=voxel_matrix, pitch=voxel_size)
# 保存为OBJ文件
output_file_obj = './output/voxel/voxel.obj'
mesh.export(output_file_obj)
print(f"Mesh exported to '{output_file_obj}'.")
###################################################################################################
voxel_matrix = voxel_matrix.astype(np.float32)
# voxel_matrix = 1-voxel_matrix
# 输出体素矩阵的信息
print("Voxel Matrix Shape:")
print(voxel_matrix.shape)
# print(voxel_matrix)

# 创建一个 64x64x64 的全零 float32 类型的三维矩阵
zero_matrix = np.zeros((64, 64, 64), dtype=np.float32)

# 计算嵌入位置
start_x = (64 - voxel_matrix.shape[0]) // 2
start_y = (64 - voxel_matrix.shape[1]) // 2
start_z = (64 - voxel_matrix.shape[2]) // 2

# 嵌入矩阵
zero_matrix[start_x:start_x+voxel_matrix.shape[0], start_y:start_y+voxel_matrix.shape[1], start_z:start_z+voxel_matrix.shape[2]] = voxel_matrix

# 输出嵌入后的矩阵
print(zero_matrix.sum())

# 计算采样点


def check_point(i, j, k, mat):
    # 初始化邻域和
    neighbor_sum = mat[i, j, k]
    
    # 初始化有效邻居计数
    valid_neighbors = 0
    
    # 定义 3x3x3 邻域内的偏移量
    for di in range(-1, 2):
        for dj in range(-1, 2):
            for dk in range(-1, 2):
                # 排除中心点自身
                if di == 0 and dj == 0 and dk == 0:
                    continue
                
                # 检查边界条件
                if 0 <= i + di < mat.shape[0] and 0 <= j + dj < mat.shape[1] and 0 <= k + dk < mat.shape[2]:
                    valid_neighbors += 1
                    neighbor_sum += mat[i + di, j + dj, k + dk]
    
    return neighbor_sum != 0 and neighbor_sum != 27, valid_neighbors

# 遍历矩阵，检查每个点
valid_points = []
for i, j, k in product(range(64), repeat=3):
    condition_met, _ = check_point(i, j, k, zero_matrix)
    if condition_met:
        valid_points.append((i, j, k))
print(f"Total valid points: {len(valid_points)}")
# 如果符合条件的点不足 32768 个，则随机采样
if len(valid_points) < 32768:
    remaining_points = list(product(range(64), repeat=3))
    for point in valid_points:
        remaining_points.remove(point)
    
    # 随机选择所需的额外点
    additional_points = np.random.choice(len(remaining_points), size=(32768 - len(valid_points)), replace=False)
    valid_points.extend([remaining_points[i] for i in additional_points])

# 现在我们有了 32768 个点
print(f"Total valid points: {len(valid_points)}")
# 按照坐标值排序
# valid_points.sort(key=lambda x: (x[0], x[1], x[2]))

# 计算矩阵中的值并拼接到坐标后面
sorted_points_with_values = [(p[0], p[1], p[2], zero_matrix[p[0], p[1], p[2]]) for p in valid_points]
sorted_points_with_values_array = np.array(sorted_points_with_values).astype(np.int8)
print(sorted_points_with_values_array[:,3].sum())

voxel_matrix_reshaped = zero_matrix.reshape(1, 64, 64, 64)
sorted_points_with_values_array_reshaped = sorted_points_with_values_array.reshape(1, 32768, 4)
# 创建 HDF5 文件
with h5py.File('./abc_all/voxel2mesh.hdf5', 'w') as f:
    # 创建 "voxels" 数据集
    f.create_dataset('voxels', data=voxel_matrix_reshaped)
    
    # 创建 "points" 数据集
    f.create_dataset('points', data=sorted_points_with_values_array_reshaped)






