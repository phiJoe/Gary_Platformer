

sceneview: main.o glad.o shader.o textureUtil.o objectCreator.o
	g++ -Llib/ -lglfw3 -lopengl32 -lkernel32 -luser32 -lgdi32 main.o glad.o shader.o textureUtil.o objectCreator.o -o sceneview.exe

main.o: main.cpp include/shader.h include/textureUtil.h include/objectCreator.h
	g++ -Iinclude -c main.cpp

shader.o: shader.cpp include/shader.h
	g++ -Iinclude -c shader.cpp

textureUtil.o: textureUtil.cpp include/textureUtil.h
	g++ -Iinclude -c textureUtil.cpp

objectCreator.o: objectCreator.cpp include/objectCreator.h
	g++ -Iinclude -c objectCreator.cpp

glad.o: glad.c
	g++ -Iinclude -c glad.c

.PHONY: clean
clean:
	rm -f $(wildcard *.o)
