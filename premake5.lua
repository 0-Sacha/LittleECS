
Solution.ProjectsInfo.Includes["LittleECS"] = {
	"%{Solution.Projects.LittleECS}/src/",
	"%{Solution.Projects.LittleECS}/src/LittleECS",
}

Solution.ProjectsInfo.Defines["LittleECS"] = {
	"LECS_ENABLE_EACH"
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

LittleECSTestLaunchPerf = {}
LittleECSTestLaunchPerf.project = "LittleECSTests"
LittleECSTestLaunchPerf.BuildCfg = "Release"
LittleECSTestLaunchPerf.Platform = "x64"
LittleECSTestLaunchPerf.PreLaunchTask = "RELEASEx64 build"
Solution.Launch["LittleECSTests-PerfTest"] = LittleECSTestLaunchPerf

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
	