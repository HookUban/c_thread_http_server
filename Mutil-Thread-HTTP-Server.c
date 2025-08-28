/*
 * 多线程HTTP服务器实现
 * 功能：处理HTML和JPEG文件的HTTP请求
 * 支持并发连接，最大同时处理10个请求
 */

#include <stdio.h>
#include <string.h>     // 字符串操作函数
#include <stdlib.h>     // 内存分配函数
#include <sys/socket.h> // Socket编程相关
#include <arpa/inet.h>  // 网络地址转换
#include <unistd.h>     // 系统调用
#include <pthread.h>    // POSIX线程库
#include <sys/types.h>  // 系统数据类型
#include <sys/stat.h>   // 文件状态
#include <fcntl.h>      // 文件控制
#include <semaphore.h>  // 信号量

// 配置常量定义
#define PATH "/home/admin/Mutil-Thread-HTTP-Server/http-sources" // 服务器根目录路径（注意：路径末尾不能有"/"）
#define PORT_NO 7888        // 服务器监听端口
#define BUFFER_SIZE 1024    // 缓冲区大小
#define CONNECTION_NUMBER 10 // 最大并发连接数

// 全局变量
int thread_count = 0;   // 当前活跃线程计数器
sem_t mutex;           // 信号量，用于保护thread_count的并发访问

/*
 * JPEG文件处理函数
 * 参数：socket - 客户端socket描述符，file_name - 请求的文件名
 * 功能：读取JPEG文件并发送给客户端
 */
void jpeg_handler(int socket, char *file_name)
{
    char *buffer;
    // 动态分配内存存储完整文件路径
    char *full_path = (char *)malloc((strlen(PATH) + strlen(file_name)) * sizeof(char));
    int fp; // 文件描述符

    // 构建完整文件路径
    strcpy(full_path, PATH);
    strcat(full_path, file_name);
    puts(full_path); // 调试输出：显示完整路径

    // 尝试打开文件（只读模式）
    if ((fp = open(full_path, O_RDONLY)) > 0) // 文件存在且成功打开
    {
        puts("Image Found."); // 调试输出：文件找到
        int bytes;
        char buffer[BUFFER_SIZE];

        // 发送HTTP响应头（200 OK，内容类型为image/jpeg）
        send(socket, "HTTP/1.0 200 OK\r\nContent-Type: image/jpeg\r\n\r\n", 45, 0);
        
        // 分块读取并发送文件内容
        while ((bytes = read(fp, buffer, BUFFER_SIZE)) > 0) // 循环读取文件直到结束
        {
            write(socket, buffer, bytes); // 将读取的数据发送给客户端
        }
    }
    else // 文件不存在或无法打开
    {
        // 发送404错误响应
        write(socket, "HTTP/1.0 404 Not Found\r\nConnection: close\r\nContent-Type: text/html\r\n\r\n<!doctype html><html><body>404 File Not Found</body></html>", strlen("HTTP/1.0 404 Not Found\r\nConnection: close\r\nContent-Type: text/html\r\n\r\n<!doctype html><html><body>404 File Not Found</body></html>"));
    }

    // 清理资源
    free(full_path);
    close(fp);
}

/*
 * HTML文件处理函数
 * 参数：socket - 客户端socket描述符，file_name - 请求的文件名
 * 功能：读取HTML文件并发送给客户端
 */
void html_handler(int socket, char *file_name)
{
    char *buffer;
    // 动态分配内存存储完整文件路径
    char *full_path = (char *)malloc((strlen(PATH) + strlen(file_name)) * sizeof(char));
    FILE *fp; // 文件指针

    // 构建完整文件路径
    strcpy(full_path, PATH);
    strcat(full_path, file_name);

    // 尝试打开文件（读取模式）
    fp = fopen(full_path, "r");
    if (fp != NULL) // 文件存在且成功打开
    {
        puts("File Found."); // 调试输出：文件找到

        // 获取文件大小
        fseek(fp, 0, SEEK_END);     // 移动到文件末尾
        long bytes_read = ftell(fp); // 获取当前位置（即文件大小）
        fseek(fp, 0, SEEK_SET);     // 回到文件开头

        // 发送HTTP响应头（200 OK，内容类型为text/html）
        send(socket, "HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n\r\n", 44, 0);
        
        // 分配缓冲区并读取整个文件
        buffer = (char *)malloc(bytes_read * sizeof(char));
        fread(buffer, bytes_read, 1, fp); // 一次性读取整个文件
        write(socket, buffer, bytes_read); // 发送文件内容给客户端
        
        // 清理资源
        free(buffer);
        fclose(fp);
    }
    else // 文件不存在或无法打开
    {
        // 发送404错误响应
        write(socket, "HTTP/1.0 404 Not Found\r\nConnection: close\r\nContent-Type: text/html\r\n\r\n<!doctype html><html><body>404 File Not Found</body></html>", strlen("HTTP/1.0 404 Not Found\r\nConnection: close\r\nContent-Type: text/html\r\n\r\n<!doctype html><html><body>404 File Not Found</body></html>"));
    }

    free(full_path); // 释放路径内存
}

/*
 * 连接处理函数（线程入口函数）
 * 参数：socket_desc - 指向socket描述符的指针
 * 功能：处理单个客户端连接的HTTP请求
 */
void *connection_handler(void *socket_desc)
{
    int request;
    char client_reply[BUFFER_SIZE], *request_lines[3]; // HTTP请求缓冲区和解析数组
    char *file_name;
    char *extension;

    // 获取socket描述符
    int sock = *((int *)socket_desc);

    // 接收客户端请求
    request = recv(sock, client_reply, BUFFER_SIZE, 0);

    // 线程计数器保护（临界区开始）
    sem_wait(&mutex);
    thread_count++; // 增加活跃线程数

    // 检查是否超过最大并发连接数
    if (thread_count > 10)
    {
        // 服务器繁忙，拒绝请求
        char *message = "HTTP/1.0 400 Bad Request\r\nContent-Type: text/html\r\n\r\n<!doctype html><html><body>System is busy right now.</body></html>";
        write(sock, message, strlen(message));
        thread_count--; // 减少线程计数
        sem_post(&mutex); // 释放信号量
        
        // 清理资源并退出线程
        free(socket_desc);
        shutdown(sock, SHUT_RDWR);
        close(sock);
        sock = -1;
        pthread_exit(NULL);
    }
    sem_post(&mutex); // 临界区结束

    // 处理接收结果
    if (request < 0) // 接收失败
    {
        puts("Recv failed");
    }
    else if (request == 0) // 客户端意外断开连接
    {
        puts("Client disconnected upexpectedly.");
    }
    else // 成功接收到消息
    {
        printf("%s", client_reply); // 调试输出：显示请求内容
        
        // 解析HTTP请求行（方法 URL 版本）
        request_lines[0] = strtok(client_reply, " \t\n"); // HTTP方法
        
        if (strncmp(request_lines[0], "GET\0", 4) == 0) // 只处理GET请求
        {
            // 继续解析请求
            request_lines[1] = strtok(NULL, " \t");    // URL路径
            request_lines[2] = strtok(NULL, " \t\n");  // HTTP版本

            // 检查HTTP版本是否支持
            if (strncmp(request_lines[2], "HTTP/1.0", 8) != 0 && strncmp(request_lines[2], "HTTP/1.1", 8) != 0)
            {
                // 不支持的HTTP版本
                char *message = "HTTP/1.0 400 Bad Request\r\nConnection: close\r\nContent-Type: text/html\r\n\r\n<!doctype html><html><body>400 Bad Request</body></html>";
                write(sock, message, strlen(message));
            }
            else
            {
                char *tokens[2]; // 用于解析文件名和扩展名

                // 复制文件名用于解析
                file_name = (char *)malloc(strlen(request_lines[1]) * sizeof(char));
                strcpy(file_name, request_lines[1]);
                puts(file_name); // 调试输出：显示请求的文件名

                // 解析文件名和扩展名
                tokens[0] = strtok(file_name, "."); // 文件名部分
                tokens[1] = strtok(NULL, ".");      // 扩展名部分

                // 忽略favicon.ico请求
                if (strcmp(tokens[0], "/favicon") == 0 && strcmp(tokens[1], "ico"))
                {
                    // 直接结束线程，不处理favicon请求
                    sem_wait(&mutex);
                    thread_count--;
                    sem_post(&mutex);
                    free(socket_desc);
                    shutdown(sock, SHUT_RDWR);
                    close(sock);
                    sock = -1;
                    pthread_exit(NULL);
                }
                else if (tokens[0] == NULL || tokens[1] == NULL) // 请求格式错误
                {
                    // 没有扩展名或请求根路径
                    char *message = "HTTP/1.0 400 Bad Request\r\nConnection: close\r\n\r\n<!doctype html><html><body>400 Bad Request. (You need to request to jpeg and html files)</body></html>";
                    write(sock, message, strlen(message));
                }
                else
                {
                    // 检查文件类型是否支持
                    if (strcmp(tokens[1], "html") != 0 && strcmp(tokens[1], "jpeg") != 0)
                    {
                        // 不支持的文件类型
                        char *message = "HTTP/1.0 400 Bad Request\r\nConnection: close\r\n\r\n<!doctype html><html><body>400 Bad Request. Not Supported File Type (Suppoerted File Types: html and jpeg)</body></html>";
                        write(sock, message, strlen(message));
                    }
                    else
                    {
                        // 根据文件类型调用相应处理函数
                        if (strcmp(tokens[1], "html") == 0)
                        {
                            sem_wait(&mutex); // 防止多个线程同时进行IO操作
                            html_handler(sock, request_lines[1]);
                            sem_post(&mutex);
                        }
                        else if (strcmp(tokens[1], "jpeg") == 0)
                        {
                            sem_wait(&mutex); // 防止多个线程同时进行IO操作
                            jpeg_handler(sock, request_lines[1]);
                            sem_post(&mutex);
                        }
                    }
                    free(extension); // 释放扩展名内存
                }
                free(file_name); // 释放文件名内存
            }
        }
    }

    // 可选：用于测试并发限制的延时
    // sleep(50); // 如果想看到只有10个线程同时工作，可以在这里睡眠
    // 发送10个请求后，第11个请求会收到"System is busy right now"响应
    
    // 清理资源并结束线程
    free(socket_desc);           // 释放socket描述符内存
    shutdown(sock, SHUT_RDWR);   // 关闭socket的读写
    close(sock);                 // 关闭socket
    sock = -1;
    
    // 减少线程计数
    sem_wait(&mutex);
    thread_count--;
    sem_post(&mutex);
    
    pthread_exit(NULL); // 退出线程
}

/*
 * 主函数
 * 功能：初始化服务器，监听连接，为每个连接创建处理线程
 */
int main(int argc, char *argv[])
{
    // 初始化信号量（初始值为1，用作互斥锁）
    sem_init(&mutex, 0, 1);
    
    int socket_desc, new_socket, c, *new_sock;
    struct sockaddr_in server, client; // 服务器和客户端地址结构

    // 创建socket
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_desc == -1)
    {
        puts("Could not create socket");
        return 1;
    }

    // 配置服务器地址
    server.sin_family = AF_INET;        // IPv4
    server.sin_addr.s_addr = INADDR_ANY; // 监听所有网络接口
    server.sin_port = htons(PORT_NO);    // 设置端口号（网络字节序）

    // 绑定socket到地址
    if (bind(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        puts("Binding failed");
        return 1;
    }

    // 开始监听连接（最大队列长度20）
    listen(socket_desc, 20);

    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);
    
    // 主循环：接受连接并创建处理线程
    while ((new_socket = accept(socket_desc, (struct sockaddr *)&client, (socklen_t *)&c)))
    {
        puts("Connection accepted \n");

        pthread_t sniffer_thread; // 线程标识符
        new_sock = malloc(1);      // 为socket描述符分配内存
        *new_sock = new_socket;    // 保存新连接的socket描述符

        // 为每个连接创建新线程
        if (pthread_create(&sniffer_thread, NULL, connection_handler, (void *)new_sock) < 0)
        {
            puts("Could not create thread");
            return 1;
        }
    }

    return 0;
}
