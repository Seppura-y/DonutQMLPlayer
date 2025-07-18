import os

def convert_to_utf8(file_path):
    """转换文件到UTF-8编码"""
    # 先检查是否已经是UTF-8
    try:
        with open(file_path, 'r', encoding='utf-8') as file:
            content = file.read()
        print(f"File {file_path} is already UTF-8")
        return
    except UnicodeDecodeError:
        pass
    
    # 尝试多种编码格式
    encodings = ['gbk', 'gb2312', 'big5', 'latin1', 'cp1252', 'iso-8859-1', 'utf-16', 'utf-32']
    
    content = None
    used_encoding = None
    
    # 尝试不同编码读取文件
    for encoding in encodings:
        try:
            with open(file_path, 'r', encoding=encoding, errors='ignore') as file:
                content = file.read()
                used_encoding = encoding
                break
        except (UnicodeDecodeError, LookupError):
            continue
    
    if content is not None:
        try:
            # 写入UTF-8格式
            with open(file_path, 'w', encoding='utf-8') as file:
                file.write(content)
            print(f"Converted {file_path} from {used_encoding} to UTF-8")
        except Exception as e:
            print(f"Failed to write {file_path}: {e}")
    else:
        print(f"Failed to decode {file_path} with any encoding")

def convert_directory_to_utf8(directory):
    """转换目录下所有源代码文件到UTF-8"""
    if not os.path.exists(directory):
        print(f"Directory {directory} does not exist!")
        return
    
    # 只处理源代码文件
    source_extensions = ('.cpp', '.h', '.c', '.hpp', '.cc', '.cxx', '.hxx')
    
    converted_count = 0
    total_count = 0
    
    for root, _, files in os.walk(directory):
        for file in files:
            if file.endswith(source_extensions):
                file_path = os.path.join(root, file)
                total_count += 1
                print(f"Processing ({total_count}): {file_path}")
                convert_to_utf8(file_path)

if __name__ == "__main__":
    src_directory = "./"
    print(f"Converting files in directory: {src_directory}")
    convert_directory_to_utf8(src_directory)
    print("Conversion completed!")