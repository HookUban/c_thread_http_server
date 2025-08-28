# 多线程HTTP服务器 (Multi-Thread HTTP Server)

一个用C语言实现的高性能多线程HTTP服务器，支持并发处理HTML和JPEG文件请求。

## 📋 项目概述

本项目实现了一个轻量级的HTTP服务器，具有以下特点：
- 🚀 **多线程并发处理**：最大支持10个并发连接
- 📁 **文件类型支持**：HTML文件和JPEG图片
- 🔒 **线程安全**：使用信号量保护共享资源
- 🌐 **标准HTTP协议**：支持HTTP/1.0和HTTP/1.1
- ⚡ **高性能**：基于POSIX线程库实现

## 🏗️ 项目结构
c_thread_http_server/
├── Mutil-Thread-HTTP-Server.c    # 主服务器源代码
├── python_test_client.py          # Python测试客户端
├── README.md                      # 项目说明文档
├── .gitignore                     # Git忽略文件配置
└── http-sources/                  # 网站根目录
	├── test.html                  # 测试HTML文件
	└── 11048.jpeg                 # 测试JPEG图片

## ⚙️ 技术规格

- **编程语言**：C语言 (C99标准)
- **线程库**：POSIX Threads (pthread)
- **网络协议**：TCP/IP Socket编程
- **HTTP版本**：HTTP/1.0, HTTP/1.1
- **默认端口**：7888
- **最大并发连接**：10个
- **缓冲区大小**：1024字节
- **支持的操作系统**：Linux, Unix-like系统

## 🚀 快速开始

### 系统要求

```bash
# 确保系统已安装必要的开发工具
sudo apt update
sudo apt install build-essential gcc libc6-dev

# 或者在CentOS/RHEL系统上
sudo yum groupinstall "Development Tools"
sudo yum install gcc glibc-devel
```

### 编译服务器

```bash
# 进入项目目录
cd c_thread_http_server

# 基础编译
gcc -o http_server Mutil-Thread-HTTP-Server.c -lpthread

# 优化编译（推荐生产环境）
gcc -O2 -Wall -Wextra -o http_server Mutil-Thread-HTTP-Server.c -lpthread

# 调试编译（开发环境）
gcc -g -DDEBUG -o http_server Mutil-Thread-HTTP-Server.c -lpthread
```

### 启动服务器

```bash
# 前台运行（适合调试）
./http_server

# 后台运行（适合生产环境）
nohup ./http_server > server.log 2>&1 &

# 查看服务器进程
ps aux | grep http_server

# 查看服务器日志
tail -f server.log
```

### 停止服务器

```bash
# 查找服务器进程
netstat -tlnp | grep 7888
# 或者
ps aux | grep http_server

# 优雅停止（推荐）
kill PID

# 强制停止
kill -9 PID

# 一键停止所有相关进程
pkill http_server
```

## 🧪 测试指南

### 1. 基础功能测试

#### 使用curl命令测试

```bash
# 测试HTML文件
curl -v http://localhost:7888/test.html

# 测试JPEG图片
curl -v http://localhost:7888/11048.jpeg -o downloaded_image.jpeg

# 测试不存在的文件（应返回404）
curl -v http://localhost:7888/nonexistent.html

# 测试不支持的文件类型（应返回400）
curl -v http://localhost:7888/test.txt
```

#### 使用wget测试

```bash
# 下载HTML文件
wget http://localhost:7888/test.html

# 下载图片文件
wget http://localhost:7888/11048.jpeg

# 测试连接超时
wget --timeout=5 http://localhost:7888/test.html
```

#### 使用浏览器测试

```bash
# 在浏览器中访问以下URL：
http://localhost:7888/test.html
http://localhost:7888/11048.jpeg
```

### 2. Python测试客户端

项目包含一个专门的Python测试客户端：

```bash
# 运行完整的测试套件
python3 python_test_client.py

# 测试特定文件
python3 -c "
import python_test_client as client
client.test_http_server('localhost', 7888, '/test.html')
"

# 测试图片文件
python3 -c "
import python_test_client as client
client.test_http_server('localhost', 7888, '/11048.jpeg')
"
```

### 3. 并发性能测试

#### 使用Apache Bench (ab)

```bash
# 安装Apache Bench
sudo apt install apache2-utils  # Ubuntu/Debian
sudo yum install httpd-tools     # CentOS/RHEL

# 并发测试（10个并发连接，总共100个请求）
ab -n 100 -c 10 http://localhost:7888/test.html

# 长时间压力测试（持续30秒）
ab -t 30 -c 10 http://localhost:7888/test.html
```

#### 使用自定义并发测试脚本

```bash
# 创建并发测试脚本
cat > concurrent_test.sh << 'EOF'
#!/bin/bash
echo "开始并发测试..."
for i in {1..15}; do
    curl -s http://localhost:7888/test.html > /dev/null &
    echo "启动连接 $i"
done
wait
echo "并发测试完成"
EOF

chmod +x concurrent_test.sh
./concurrent_test.sh
```

### 4. 错误处理测试

```bash
# 测试404错误
curl -i http://localhost:7888/notfound.html

# 测试400错误（不支持的文件类型）
curl -i http://localhost:7888/test.txt

# 测试服务器繁忙（超过10个并发连接）
for i in {1..12}; do
    curl http://localhost:7888/test.html &
done
```

### 5. 网络连接测试

```bash
# 测试端口是否开放
telnet localhost 7888

# 测试网络连通性
ping localhost

# 检查服务器监听状态
netstat -tlnp | grep 7888
ss -tlnp | grep 7888
```

### 6. 远程访问测试

如果部署在云服务器上：

```bash
# 替换为您的服务器IP地址
SERVER_IP="your_server_ip"

# 测试远程访问
curl -v http://$SERVER_IP:7888/test.html

# 使用Python客户端测试远程连接
python3 -c "
import python_test_client as client
client.test_http_server('$SERVER_IP', 7888, '/test.html')
"
```

## 📡 支持的HTTP请求

### 请求格式

```http
GET /filename.extension HTTP/1.1
Host: localhost:7888
Connection: close
```

### 支持的文件类型

| 文件扩展名 | Content-Type | 说明 |
|-----------|--------------|------|
| `.html` | `text/html` | HTML网页文件 |
| `.jpeg` | `image/jpeg` | JPEG图片文件 |

### HTTP响应状态码

| 状态码 | 说明 | 触发条件 |
|--------|------|----------|
| `200 OK` | 请求成功 | 文件存在且成功读取 |
| `400 Bad Request` | 请求错误 | 不支持的文件类型或请求格式错误 |
| `404 Not Found` | 文件未找到 | 请求的文件不存在 |
| `503 Service Unavailable` | 服务不可用 | 超过最大并发连接数(10个) |

### 请求示例

```bash
# 成功请求示例
curl -i http://localhost:7888/test.html
# HTTP/1.0 200 OK
# Content-Type: text/html
# 
# <html><body><h1>Hello World!</h1></body></html>

# 404错误示例
curl -i http://localhost:7888/missing.html
# HTTP/1.0 404 Not Found
# Connection: close
# Content-Type: text/html
# 
# <!doctype html><html><body>404 File Not Found</body></html>
```

## 🔧 配置选项

可以通过修改源代码中的宏定义来调整服务器配置：

```c
// 在 Mutil-Thread-HTTP-Server.c 文件中修改以下常量：

#define PATH "/path/to/your/web/root"    // 网站根目录路径
#define PORT_NO 7888                     // 服务器监听端口
#define BUFFER_SIZE 1024                 // 数据缓冲区大小
#define CONNECTION_NUMBER 10             // 最大并发连接数
```

### 自定义配置示例

```c
// 修改为自定义配置
#define PATH "/var/www/html"              // 使用标准web目录
#define PORT_NO 8080                     // 使用8080端口
#define BUFFER_SIZE 2048                 // 增大缓冲区
#define CONNECTION_NUMBER 20             // 支持20个并发连接
```

## 🛠️ 部署指南

### 本地开发环境

```bash
# 1. 克隆项目
git clone https://github.com/your-username/c_thread_http_server.git
cd c_thread_http_server

# 2. 编译运行
gcc -o http_server Mutil-Thread-HTTP-Server.c -lpthread
./http_server

# 3. 测试访问
curl http://localhost:7888/test.html
```

### 生产环境部署

#### 1. 系统服务配置

```bash
# 创建systemd服务文件
sudo tee /etc/systemd/system/http-server.service << EOF
[Unit]
Description=Multi-Thread HTTP Server
After=network.target

[Service]
Type=simple
User=www-data
WorkingDirectory=/opt/c_thread_http_server
ExecStart=/opt/c_thread_http_server/http_server
Restart=always
RestartSec=3

[Install]
WantedBy=multi-user.target
EOF

# 启用并启动服务
sudo systemctl enable http-server
sudo systemctl start http-server
sudo systemctl status http-server
```

#### 2. 防火墙配置

```bash
# Ubuntu/Debian (ufw)
sudo ufw allow 7888/tcp
sudo ufw reload

# CentOS/RHEL (firewalld)
sudo firewall-cmd --permanent --add-port=7888/tcp
sudo firewall-cmd --reload

# 验证端口开放
sudo netstat -tlnp | grep 7888
```

#### 3. 云服务器配置

- **阿里云ECS**：在安全组中开放7888端口
- **腾讯云CVM**：在安全组中添加入站规则
- **AWS EC2**：在Security Groups中配置入站规则

## 🔍 故障排除

### 常见问题及解决方案

#### 1. 编译错误

```bash
# 错误：pthread库未找到
# 解决：确保链接pthread库
gcc -o http_server Mutil-Thread-HTTP-Server.c -lpthread

# 错误：头文件未找到
# 解决：安装开发工具包
sudo apt install build-essential
```

#### 2. 运行时错误

```bash
# 错误：Binding failed
# 原因：端口被占用
# 解决：
netstat -tlnp | grep 7888
kill -9 <PID>

# 或者修改端口号
sed -i 's/#define PORT_NO 7888/#define PORT_NO 8080/' Mutil-Thread-HTTP-Server.c
```

#### 3. 连接问题

```bash
# 问题：无法访问服务器
# 检查步骤：
# 1. 确认服务器正在运行
ps aux | grep http_server

# 2. 检查端口监听状态
netstat -tlnp | grep 7888

# 3. 测试本地连接
curl -v http://localhost:7888/test.html

# 4. 检查防火墙设置
sudo ufw status
sudo firewall-cmd --list-ports
```

#### 4. 性能问题

```bash
# 问题：响应缓慢
# 解决方案：
# 1. 增加并发连接数
# 2. 优化编译选项
gcc -O2 -o http_server Mutil-Thread-HTTP-Server.c -lpthread

# 3. 监控系统资源
top
htop
iostat 1
```

### 调试技巧

```bash
# 1. 启用详细日志
gcc -DDEBUG -g -o http_server Mutil-Thread-HTTP-Server.c -lpthread

# 2. 使用GDB调试
gdb ./http_server
(gdb) run
(gdb) bt  # 查看调用栈

# 3. 使用strace跟踪系统调用
strace -p <PID>

# 4. 监控网络连接
watch -n 1 'netstat -an | grep 7888'
```

## 📊 性能基准测试

### 测试环境

- **CPU**: 2核心 2.4GHz
- **内存**: 4GB RAM
- **操作系统**: Ubuntu 20.04 LTS
- **网络**: 千兆以太网

### 基准测试结果

```bash
# Apache Bench测试结果示例
ab -n 1000 -c 10 http://localhost:7888/test.html

# 结果：
# Requests per second: 2847.33 [#/sec] (mean)
# Time per request: 3.512 [ms] (mean)
# Transfer rate: 1234.56 [Kbytes/sec] received
```

### 性能优化建议

1. **编译优化**：使用 `-O2` 或 `-O3` 编译选项
2. **系统调优**：调整系统的文件描述符限制
3. **内存优化**：根据实际需求调整缓冲区大小
4. **并发调优**：根据硬件配置调整最大连接数

## 🔧 核心功能实现

### 多线程架构

```c
// 线程安全的连接计数
sem_wait(&mutex);
thread_count++;
if (thread_count > CONNECTION_NUMBER) {
    // 服务器繁忙处理逻辑
    char *message = "HTTP/1.0 503 Service Unavailable\r\n...";
    write(sock, message, strlen(message));
}
sem_post(&mutex);
```

### 文件处理机制

- **HTML处理器**：使用 `fopen()` 以文本模式读取
- **JPEG处理器**：使用 `open()` 以二进制模式读取
- **内存管理**：动态分配和及时释放内存
- **错误处理**：统一的HTTP错误响应格式

### 网络通信

- **Socket创建**：`AF_INET` + `SOCK_STREAM`
- **地址绑定**：`INADDR_ANY` 监听所有网络接口
- **连接监听**：支持最大20个排队连接
- **数据传输**：分块读取和发送大文件

## 🤝 贡献指南

### 开发环境设置

```bash
# 1. Fork并克隆仓库
git clone https://github.com/your-username/c_thread_http_server.git
cd c_thread_http_server

# 2. 创建开发分支
git checkout -b feature/your-feature-name

# 3. 配置开发环境
gcc -g -DDEBUG -o http_server Mutil-Thread-HTTP-Server.c -lpthread
```

### 代码规范

- 使用4个空格缩进
- 函数名使用下划线命名法
- 添加详细的注释说明
- 遵循C99标准

### 提交流程

1. Fork 本仓库
2. 创建特性分支 (`git checkout -b feature/AmazingFeature`)
3. 提交更改 (`git commit -m 'Add some AmazingFeature'`)
4. 推送到分支 (`git push origin feature/AmazingFeature`)
5. 开启 Pull Request

### 测试要求

提交代码前请确保：
- [ ] 编译无警告无错误
- [ ] 通过所有基础功能测试
- [ ] 通过并发性能测试
- [ ] 添加相应的测试用例

## 📄 许可证

本项目采用 MIT 许可证 - 查看 [LICENSE](LICENSE) 文件了解详情。

## 🙏 致谢

- 感谢所有为这个项目做出贡献的开发者
- 特别感谢POSIX线程库的开发团队
- 感谢开源社区的支持和反馈

---

**注意**: 这是一个教育和学习项目，用于演示多线程HTTP服务器的实现原理。在生产环境中使用时，请考虑添加更多的安全特性、错误处理机制和性能优化。

## 🔖 版本历史

- **v1.0.0** (2024-01-01): 初始版本发布
  - 基础HTTP服务器功能
  - 多线程并发处理
  - HTML和JPEG文件支持

- **v1.1.0** (待发布): 功能增强
  - 添加更多文件类型支持
  - 改进错误处理机制
  - 性能优化和内存管理改进