# Episode One

## Preface
What this is

    - A pong tutorial
    - The basics of graphics programming
    - Triangle and Quad rendering using OpenGL

What this isn't

    - A C++ tutorial
    - A GLFW/GLEW installation tutorial (you should already have these downloaded and linked)

The tutorial is written in C++, but doesn't use any/many c++ specific features, so it's pretty easy to translate to other languages. 

## What is OpenGL
OpenGL is a Graphics API. Graphics APIs are the middleman between direct messaging with your GPU drivers, and high level rendering.

Graphics APIs abstract all of the nasty driver code with nice, clean, and (relativly) simple alternatives.

The main graphics APIs that you will encounter are OpenGL, Vulkan, DirectX, and Metal. While there are others, they either aren't as widely supported or aren't as popular.

## Why OpenGL is Used
OpenGL is the easiest out of all mainstream graphics APIs. It allows for most of the features of newer APIs like vulkan, while being way easier for begineers.

The latest version of OpenGL is version 4.6, and it was released in 2017. This version predates raytracing, but it is capable of rendering almost anything else.

This tutorial will only be using OpenGL 3.3 until we cover more advanced topics like SPIR-V and compute shaders.

OpenGL is a good stepping stone between using game engines like Unity and Godot, and using high level graphics APIs like Vulkan and DirectX12.

## Where to Start
To start with OpenGL, you want to get a framebuffer and load all of the OpenGL functions from your system. There are many libraries that do this.

In my experience, GLFW is the best library for handling window/framebuffer creation, and GLEW is the best OpenGL loader. Both of these libraries are cross platform and have simple to learn and well documented APIs.

## Window Creation
This tutorial assumes that you are using GLFW, if you are not, skip this part and reimplement the GLFW parts with your library of choice.

We can start by using GLFW to create a window by using the following functions.

```cpp
glfwInit();
GLFWwindow* window = glfwCreateWindow(width, height, name, monitor, share);
```

This initilizes GLFW and creates a handle to the window for further use.

Width, Height, and name should be self-explanitory.

Monitor is the monitor to use if we want a full screen window. Since we don't want a maximized window, we can just set it to NULL.

Share is used for when you want multiple windows that share resources with each other. Just like monitor, we can set this to NULL since we aren't using it.

If we want support for OpenGL, we need an OpenGL context. Thankfully, GLFW creates one for us with the following function.

```cpp
glfwMakeContextCurrent(window);
```

This allows for GLEW (or any other OpenGL loader that you want to use) to load OpenGL functions.

Now, if you run this, you will notice that we don't have a window.

This is due to us creating the window an immediately shutting down.

We can prevent this by doing the following.

```cpp
while (glfwWindowShouldClose(window) == false) {
	// put main loop here
}
```

This keeps the window open until we tell it to close (eg when pressing alt+F4 or closing it with the menu bar.

Well now the application doesn't close immediately but we still don't have a window. The fun isn't over just yet.

We need to tell GLFW to check all of the incoming events and process them.

We can do this using `glfwPollEvents()` in our main loop.

We also need to tell GLFW to swap the front and back framebuffers. This is done using `glfwSwapBuffers(window)` at the end of our main loop.

And there we have it. You should now see a window with a black background.

Before we get into actual OpenGL, we need to make sure that we clean up GLFW. This can be done by putting the following at the end of our file.

```cpp
glfwDestroyWindow(window);
glfwTerminate();
```

This cleans up GLFW and prevents memory leaks from occuring (which is always a good thing).

## Initilizing OpenGL
This is probably going to be the easiest step. All you have to do is call one function.

```cpp
glewInit();
```

Thats it. We now have access to OpenGL :sunglasses:

## Clearing the Screen
If we want to actually render things, we must clear the back buffer before we do any actual rendering.

OpenGL allows us to do this using the function `glClear`. We can also specify which buffers to clear, and since we only want to clear color, we pass `GL_COLOR_BUFFER_BIT` to it. We put this before `glfwSwapBuffers` but after `glfwPollEvents`.

While black can look nice, I think we should get another color as the background of our window.

To set the color that OpenGL clears the screen too, we call the following function before `glClear`.

```cpp
// RGBA, this puts it as a nice blue color
glSetClearColor(0.2f, 0.3f, 0.7f, 1.0f);
```

## Recap

This is what our main function should roughly look like by now.

```cpp
glfwInit();
GLFWwindow* window = glfwCreateWindow(1280, 720, "Episode One", NULL, NULL);
glfwMakeContextCurrent(window);

glewInit();

while (glfwWindowShouldClose(window) == false) {
	glfwPollEvents();

	glClearColor(0.2f, 0.3f, 0.7f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glfwSwapBuffers(window);
}

glfwDestroyWindow(window);
glfwTerminate();
```

This creates a window and paints the background blue. Cool, now we can get to the fun stuff.

## The Graphics Pipeline
The graphics pipeline is the steps that your GPU takes to render a triangle. It takes in an array of vertices and outputs an image that gets displayed on your monitor. 

```
 ┌────────────────────┐┌─────────────┐      ┌───────────────┐┌──────────────┐     ┌─────────────┐      ┌───────────────┐         ┌───────┐           ┌───────┐
 │Vertices and Indices││Vertex Shader│      │Geometry Shader││Shape Assembly│     │Rasterization│      │Fragment Shader│         │Testing│           │Monitor│
 └─────────┬──────────┘└──────┬──────┘      └───────┬───────┘└──────┬───────┘     └──────┬──────┘      └───────┬───────┘         └───┬───┘           └───┬───┘
           │                  │                     │               │                    │                     │                     │                   │    
           │   Vertex Data    │                     │               │                    │                     │                     │                   │    
           │─────────────────>│                     │               │                    │                     │                     │                   │    
           │                  │                     │               │                    │                     │                     │                   │    
           │                  │Processed Vertex Data│               │                    │                     │                     │                   │    
           │                  │────────────────────>│               │                    │                     │                     │                   │    
           │                  │                     │               │                    │                     │                     │                   │    
           │                  │                     │ Triangle Data │                    │                     │                     │                   │    
           │                  │                     │──────────────>│                    │                     │                     │                   │    
           │                  │                     │               │                    │                     │                     │                   │    
           │                  │                     │               │Assembled Shape Data│                     │                     │                   │    
           │                  │                     │               │───────────────────>│                     │                     │                   │    
           │                  │                     │               │                    │                     │                     │                   │    
           │                  │                     │               │                    │Pixels to Render     │                     │                   │    
           │                  │                     │               │                    │to With Fragment Info│                     │                   │    
           │                  │                     │               │                    │────────────────────>│                     │                   │    
           │                  │                     │               │                    │                     │                     │                   │    
           │                  │                     │               │                    │                     │Array of Pixels      │                   │    
           │                  │                     │               │                    │                     │that Contains the    │                   │    
           │                  │                     │               │                    │                     │Newly Drawn Primitive│                   │    
           │                  │                     │               │                    │                     │────────────────────>│                   │    
           │                  │                     │               │                    │                     │                     │                   │    
           │                  │                     │               │                    │                     │                     │Tests if Triangle  │    
           │                  │                     │               │                    │                     │                     │Should Be Displayed│    
           │                  │                     │               │                    │                     │                     │──────────────────>│    
 ┌─────────┴──────────┐┌──────┴──────┐      ┌───────┴───────┐┌──────┴───────┐     ┌──────┴──────┐      ┌───────┴───────┐         ┌───┴───┐           ┌───┴───┐
 │Vertices and Indices││Vertex Shader│      │Geometry Shader││Shape Assembly│     │Rasterization│      │Fragment Shader│         │Testing│           │Monitor│
 └────────────────────┘└─────────────┘      └───────────────┘└──────────────┘     └─────────────┘      └───────────────┘         └───────┘           └───────┘

 
```

The only stages that are programmable by us are the Vertex Shader, the Geometry Shader, and the Fragment Shader. The Geometry Shader is optional, but the other two are required or else nothing will draw.

There is also a type of shader called the Compute Shader. It runs outside of the graphics pipeline and is used for when you need to run a program using the GPU using inputs and outputs that differ from graphics rendering. This could be a raytracer using a few inputs to describe shapes and outputs a texture, or really anything you can think of that needs to be parallelized.

## Shader Creation
We will go into more detail onto creating shaders in a later episode, but heres the basics.

A shader is a program that runs on the GPU. Simple as that.

In OpenGL, shaders are written in a language called GLSL. Its syntax is simular to C. We take GLSL code and compile it before we use the shader to draw anything.

Below is our first Vertex Shader, it takes in a `vec3` that serves as the x,y,z of the vertex we are processing.

```glsl
#version 330 core /// #version is always required to be the first line of any GLSL shader.

/// position of the vertex we are processing
layout (location = 0) in vec3 in_pos;

void main() {
    	/// gl_Position is the position of the vertex on the screen
    	/// we will do more with the vertex shader later on in the series
	gl_Position = vec4(in_pos, 1);
}
```

Now we want to put this inside of a C string, so then we can use it with OpenGL.

```cpp
static const char* vertex_shader = 
"#version 330 core\n"
"layout (location = 0) in vec3 in_pos;\n"
"void main() {\n"
"    gl_Position = vec4(in_pos, 1);\n"
"}\0";
```

The Fragment Shader isn't too much different from the Vertex Shader. However, instead of outputing to `gl_Position` we create a new variable with the `out` keyword and put the color value into that

```glsl
#version 330 core

/// the color we want to display
out vec4 color;

void main() {
	/// display green
	color = vec4(0.2f, 0.7f, 0.3f, 1.0f);
}
```

Just like the vertex shader, we want this in C string format

```cpp
static const char* fragment_shader =
"#version 330 core\n"
"out vec4 color;\n"
"void main() {\n"
"   color = vec4(0.2f, 0.7f, 0.3f, 1.0f);\n"
"}\0";
```

## Shader Compilation
To compile a shader, were going to need the following three functions

```cpp
/// Check if the shader was compiled correctly
bool check_shader_compilation(unsigned int shader) {
	// ...
}

/// Compile the shader
unsigned int compile_shader(const int type, const char* value) {
	// ...
}

/// Assemble the Shaders into a Program Object (i'll explain later)
unsigned int create_program(unsigned int vertex, unsigned int fragment) {
	// ...
}
```

Now, how do we actually compile a Shader?

We need to create a shader object and tell OpenGL where the source of the shader is. Then we simply compile the shader using `glCompileShader`

You can attach multiple shaders to one shader object, but we won't be doing that

```cpp
unsigned int compile_shader(const int type, const char* value) {
	unsigned int shader = glCreateShader(type);
	glShaderSource(shader, 1, &value, NULL);
	glCompileShader(shader);

	/// Ensure that the shader compiled correctly before returning it
	return check_shader_compilation(shader) ? shader : 0;
}
```

To check if a shader is compiled correctly, we get the status of the shader using `glGetShaderiv` and if it fails to compile, we can get the error message using `glGetShaderInfoLog`

```cpp
bool check_shader_compilation(unsigned int shader) {
	int success;
	char info_log[512];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(shader, 512, NULL, info_log);
		std::cout << "Compilation Failed\n" << info_log << "\n";
	}

	return success;
}
```

If we want to actually use the compiled shaders when rendering, we need to link them into a Shader Program. A shader program contains all of the info needed to run the programmed graphics pipeline.

To create one, we call `glCreateProgram` and attach all of the shaders, then link them together using `glLinkProgram`

```cpp
unsigned int program = glCreateProgram();
glAttachShader(program, vertex);
glAttachShader(program, fragment);
glLinkProgram(program);
```

Since we no longer need the Shader Objects, we can delete those once we link the program

```cpp
glDeleteShader(vertex);
glDeleteShader(fragment);
```

So now the `create_program` function should look something like this

```cpp
unsigned int create_program(unsigned int vertex, unsigned int fragment) {
	unsigned int program = glCreateProgram();
	glAttachShader(program, vertex);
	glAttachShader(program, fragment);
	glLinkProgram(program);

	glDeleteShader(vertex);
	glDeleteShader(fragment);

	return program;
}
```


