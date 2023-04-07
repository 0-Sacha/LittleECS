
Solution.ProjectsInfo.Includes["LittleECS"] = {
	"%{Solution.Projects.LittleECS}/src/",
	"%{Solution.Projects.LittleECS}/src/LittleECS",
}

Solution.ProjectsInfo.PlatformDefineName["LittleECS"] = "LECS"

project "LittleECS"
	kind "StaticLib"
	language "C++"
	cppdialect "C++20"
	staticruntime "on"

	Solution.HighWarnings()

	targetdir 	(Solution.Path.ProjectTargetDirectory)
	objdir 		(Solution.Path.ProjectObjectDirectory)

	files {
		"src/**.h",
		"src/**.hpp",
		"src/**.inl",
		"src/**.cpp",
	}
	
	Solution.IncludeProject("LittleECS")
	Solution.IncludeAndLinkProject("ProjectCore")

if (LittleECSTestsEnable)
then

LittleECSTestLaunch = {}
LittleECSTestLaunch.project = "LittleECSTests"

Solution.Launch["LittleECSTests"] = LittleECSTestLaunch

project "LittleECSTests"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"
	staticruntime "on"

	Solution.HighWarnings()

	targetdir 	(Solution.Path.ProjectTargetDirectory)
	objdir 		(Solution.Path.ProjectObjectDirectory)

	files {
		"Tests/**.h",
		"Tests/**.hpp",
		"Tests/**.inl",
		"Tests/**.cpp",
	}
	
	Solution.IncludeProject("LittleECS")
	Solution.IncludeAndLinkProject("ProjectCore")
end
	