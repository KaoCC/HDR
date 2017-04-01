
if os.is("windows") then
    local opencvpath = os.getenv("OPENCV_DIR")

    if (opencvpath) then

        opencvpath = string.gsub(opencvpath, "\\", "/");
        local cvIncludePath = string.format("%s/../../include",opencvpath);
        local cvLibPath64 = string.format("%s/lib",opencvpath);

        print("OpenCV_Path", opencvpath)

        if (opencvpath) then
            includedirs {cvIncludePath}

            configuration "x64"
                libdirs {cvLibPath64}
            configuration{}

            links {"opencv_world320"}

        end


    else

        print("OpenCV_Path not found !")

    end


end