project "Theorem"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
    staticruntime "off"

    targetdir ("Bin/" .. OutputDir .. "/%{prj.name}")
    objdir ("Bin-Int/" .. OutputDir .. "/%{prj.name}")

    files {
        "src/**.h",
        "src/**.cpp",
    }

    vpaths {
        ["src/*"] = "src/**"
    }

    includedirs {
        "src",
        "%{wks.location}/Axiom/src",
        "%{wks.location}/Axiom/%{IncludeDir.glm}",
        "%{wks.location}/Axiom/%{IncludeDir.glfw}",
        "%{wks.location}/Axiom/%{IncludeDir.vulkan}",
        "%{wks.location}/Axiom/%{IncludeDir.spdlog}",
    }

    links {
        "Axiom",
    }

    filter "configurations:Debug"
        defines { "DEBUG" }
        runtime "Debug"
        symbols "On"

    filter "configurations:Release"
        defines { "RELEASE" }
        runtime "Release"
        optimize "On"
        symbols "On"