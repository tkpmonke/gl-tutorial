project "episode-one"
	kind "ConsoleApp"
	language "C++"
	cppdialect "c++17"
	targetdir "../bin"
	objdir "../obj"
	files { "**.cpp" }
	
	links { "GLEW", "glfw" }

	filter "system:Linux"
		links { "GL" }
	filter "system:Windows"
		links { "opengl32" }

	filter "configurations:Debug"
		defines { "DEBUG" }
		symbols "On"

	filter "configurations:Release"
		  defines { "NDEBUG" }
		  optimize "On"
		  symbols "Off"
