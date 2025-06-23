# Tên file nguồn (không cần .c)
TARGET = fake_ls

# Trình biên dịch
CC = gcc

# Tùy chọn biên dịch
CFLAGS = -Wall -O2

# Tập tin đích (output)
$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).c

# Lệnh làm sạch các file biên dịch
clean:
	rm -f $(TARGET)
