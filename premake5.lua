
Solution.Projects["LittleECS"].PlatformDefineName = "LECS"
Solution.Projects["LittleECS"].Type = "StaticLib"
Solution.Projects["LittleECS"].HeaderOnly = true
Solution.Projects["LittleECS"].IncludeDirs = {
	"%{Solution.Projects.LittleECS.Path}/src/",
	"%{Solution.Projects.LittleECS.Path}/src/LittleECS",
}
Solution.Projects["LittleECS"].Defines = {
	"LECS_ENABLE_EACH"
}
Solution.Projects["LittleECS"].ProjectDependencies = {
	"ProjectCore"
}

project "LittleECS"
	kind 		(Solution.Projects["ProjectCore"].Type)
	language "C++"
	cppdialect "C++20"

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

Solution.AddProject("LittleECSTests", Solution.Projects["LittleECS"].Path)
Solution.Projects["LittleECSTests"].ProjectDependencies = {
	"LittleECS"
}

LittleECSTestLaunch = {}
LittleECSTestLaunch.Project = "LittleECSTests"
Solution.Launch["LittleECSTests"] = LittleECSTestLaunch

LittleECSTestLaunchPerf = {}
LittleECSTestLaunchPerf.Project = "LittleECSTests"
LittleECSTestLaunchPerf.BuildCfg = "Release"
LittleECSTestLaunchPerf.Platform = "x64"
LittleECSTestLaunchPerf.PreLaunchTask = "RELEASEx64 build"
Solution.Launch["LittleECSTests-PerfTest"] = LittleECSTestLaunchPerf

project "LittleECSTests"
	kind 		(Solution.Projects["LittleECSTests"].Type)
	language "C++"
	cppdialect "C++20"

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
	