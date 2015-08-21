import os
def ListFilesToTxt(dir,file,wildcard,recursion):
    exts = wildcard.split(" ")
    files = os.listdir(dir)
    for name in files:
        fullname=os.path.join(dir,name)
        if(os.path.isdir(fullname) & recursion):
            ListFilesToTxt(fullname,file,wildcard,recursion)
        else:
            for ext in exts:
                if(name.endswith(ext)):
                    str1 = dir[1:]
                    str = os.getcwd()+str1+"\\"+name + "\n"
                    file.write(str)
                    break
def Test():
  dir="."
  outfile="filelist.txt"
  wildcard = ".jpg"
  
  file = open(outfile,"w")
  if not file:
    print ("cannot open the file %s for writing" % outfile)
  ListFilesToTxt(dir,file,wildcard, 1)
  
  file.close()
Test()