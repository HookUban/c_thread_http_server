import socket
import sys

def test_http_server(server_ip, server_port, file_path):
    """
    测试HTTP服务器的Python客户端
    
    Args:
        server_ip: 服务器IP地址
        server_port: 服务器端口
        file_path: 请求的文件路径（如 /test.html）
    """
    try:
        # 创建socket连接
        client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        client_socket.settimeout(10)  # 设置10秒超时
        
        print(f"正在连接到服务器 {server_ip}:{server_port}...")
        
        # 连接到服务器
        client_socket.connect((server_ip, server_port))
        print("连接成功！")
        
        # 构造HTTP GET请求
        http_request = f"GET {file_path} HTTP/1.1\r\nHost: {server_ip}\r\nConnection: close\r\n\r\n"
        
        print(f"发送请求：\n{http_request}")
        
        # 发送请求
        client_socket.send(http_request.encode('utf-8'))
        
        # 接收响应
        response = b""
        while True:
            data = client_socket.recv(4096)
            if not data:
                break
            response += data
        
        # 解码并显示响应
        response_str = response.decode('utf-8', errors='ignore')
        print("\n服务器响应：")
        print("=" * 50)
        print(response_str)
        print("=" * 50)
        
        # 分析响应
        if "200 OK" in response_str:
            print("\n✅ 请求成功！服务器正常工作。")
        elif "404 Not Found" in response_str:
            print("\n⚠️  文件未找到，请检查文件是否存在于服务器的http-sources目录中。")
        elif "400 Bad Request" in response_str:
            print("\n⚠️  请求格式错误或服务器繁忙。")
        else:
            print("\n❓ 收到未知响应。")
            
    except socket.timeout:
        print("❌ 连接超时！请检查：")
        print("   1. 服务器是否正在运行")
        print("   2. IP地址和端口是否正确")
        print("   3. 防火墙是否阻止了连接")
    except ConnectionRefusedError:
        print("❌ 连接被拒绝！请检查：")
        print("   1. 服务器是否正在运行")
        print("   2. 端口7888是否正确")
    except Exception as e:
        print(f"❌ 发生错误: {e}")
    finally:
        client_socket.close()
        print("\n连接已关闭。")

def main():

    # 服务器配置
    SERVER_IP = "8.148.76.29"  # 您的服务器IP
    SERVER_PORT = 7888         # 服务器端口
    
    print("=== HTTP服务器测试客户端 ===")
    print(f"目标服务器: {SERVER_IP}:{SERVER_PORT}")
    print()
    
    # 测试不同的文件请求
    test_files = [
        "/test.html",     # HTML文件测试
        "/11048.jpeg",    # JPEG文件测试（如果存在）
        "/nonexistent.html"  # 404测试
    ]
    
    for file_path in test_files:
        print(f"\n{'='*60}")
        print(f"测试文件: {file_path}")
        print(f"{'='*60}")
        test_http_server(SERVER_IP, SERVER_PORT, file_path)
        
        # 询问是否继续
        if file_path != test_files[-1]:
            input("\n按Enter键继续下一个测试...")

if __name__ == "__main__":
    main()
