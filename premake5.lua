
Solution.ProjectsInfo.Includes["LittleECS"] = {
	"%{Solution.Projects.LittleECS}/src/",
	"%{Solution.Projects.LittleECS}/src/LittleECS",
}

Solution.ProjectsInfo.Defines["LittleECS"] = {
	"LECS_ENABLE_EACH"
}

Solution.ProjectsInfo.HeaderOnly["LittleECS"] = true;

Solution.ProjectsInfo.PlatformDefineName["LittleECS"] = "LECS"

Solution.ProjectsInfo.ProjectDependencies["LittleECS"] = {
	"ProjectCore"
}

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
	
	Solution.Project("LittleECS")

if (LittleECSTestsEnable)
then

Solution.Projects["LittleECSTests"] = Solution.Projects["LittleECS"]

LittleECSTestLaunch = {}
LittleECSTestLaunch.project = "LittleECSTests"
Solution.Launch["LittleECSTests"] = LittleECSTestLaunch

LittleECSTestLaunchPerf = {}
LittleECSTestLaunchPerf.project = "LittleECSTests"
LittleECSTestLaunchPerf.BuildCfg = "Release"
LittleECSTestLaunchPerf.Platform = "x64"
LittleECSTestLaunchPerf.PreLaunchTask = "RELEASEx64 build"
Solution.Launch["LittleECSTests-PerfTest"] = LittleECSTestLaunchPerf

Solution.ProjectsInfo.ProjectDependencies["LittleECSTests"] = {
	"LittleECS"
}

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

	Solution.Project("LittleECSTests")


end
	