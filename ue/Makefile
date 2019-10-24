TARGET :=	ue

$(TARGET): $(TARGET).c
	gcc $(TARGET).c -O2 -fomit-frame-pointer -o $(TARGET)
	strip $(TARGET)
	ls -al $(TARGET)

clean:
	-rm -f $(TARGET)