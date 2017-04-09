

workspace "HDR"

    location "Generated"

    language "C++"

    configurations {"Debug", "Release"}
    platforms {"x64", "x32"}

    filter { "platforms:x64" }
        architecture "x86_64"
    filter { "platforms:x32" }
        architecture "x86"
    
    filter { }


    filter { "configurations:Debug" }
        symbols "On"

    filter { "configurations:Release" }
        -- Release should be optimized
        optimize "On"

    -- Reset the filter for other settings
    filter { }


    targetdir ("Build/Bin/%{prj.name}/%{cfg.buildcfg}/%{cfg.platform}")
    objdir ("Build/Obj/%{prj.name}/%{cfg.shortname}/%{cfg.platfrom}")


    dofile ("./OpenCVConfig.lua" )

    -- The HDR viewer

    project "HDRI"

        kind "ConsoleApp"
        includedirs "HDRI/include"

        files {"HDRI/**.cpp", "HDRI/**.hpp","HDRI/**.cl"}








