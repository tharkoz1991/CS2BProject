#include "FileProcessingSystem.h"

/* DEFAULT CONSTRUCTOR */
FileProcessingSystem::FileProcessingSystem() { }

/* DESTRUCTOR */
FileProcessingSystem::~FileProcessingSystem() 
{
   // free all file pointers
   File* p_file;
   vector<File*>::iterator vItr;
   map<string, vector<File*>>::iterator mItr = file_map.begin();
   while (mItr != file_map.end())
   {
      vItr = (mItr->second).begin();
      while (vItr != (mItr->second).end())
      {
         p_file = *vItr;
         delete p_file;
      }
      (mItr->second).clear();
   }
   file_map.clear();
}

/* METHOD TO PRINT FILE INFO TO SCREEN */
void FileProcessingSystem::BrowseFiles()
{
   if (file_map.empty())
   {
      cout << "No files" << endl;
      return;
   }

   File* p_file;
   vector<File*>::iterator vItr;
   map<string, vector<File*>>::iterator mItr = file_map.begin();
   while (mItr != file_map.end())
   {
      vItr = (mItr->second).begin();
      while (vItr != (mItr->second).end())
      {
         p_file = *vItr;
         p_file->Display();
         ++vItr;
      }
      ++mItr;
   }
}

/* METHOD TO SEARCH FILE BY ITS NAME */
File* FileProcessingSystem::SearchFile(const string name)
{
   File* p_file;
   vector<File*>::iterator vItr;
   map<string, vector<File*>>::iterator mItr = file_map.begin();
   while (mItr != file_map.end())
   {
      vItr = (mItr->second).begin();
      while (vItr != (mItr->second).end())
      {
         p_file = *vItr;
         if (p_file->get_name() == name)
            return p_file;
      }
   }
   return NULL;
}

/* METHOD TO DELETE FILE BY POINTER */
void FileProcessingSystem::DeleteFile (File* file)
{
   DeleteFile(file->get_name());
}

/* METHOD TO DELETE FILE BY NAME */
void FileProcessingSystem::DeleteFile (string file_name)
{
   File* p_file;
   vector<File*>::iterator vItr;
   map<string, vector<File*>>::iterator mItr = file_map.begin();
   while (mItr != file_map.end())
   {
      vItr = (mItr->second).begin();
      while (vItr != (mItr->second).end())
      {
         p_file = *vItr;
         if (p_file->get_name() == file_name)
         {
            delete p_file;
            (mItr->second).erase(vItr);
            return;
         }
      }
   }
}

/* METHOD TO OPEN FILE */
void FileProcessingSystem::OpenFile(string name)
{
   File* p_file;
   vector<File*>::iterator vItr;
   map<string, vector<File*>>::iterator mItr = file_map.begin();
   while (mItr != file_map.end())
   {
      vItr = (mItr->second).begin();
      while (vItr != (mItr->second).end())
      {
         p_file = *vItr;
         if (p_file->get_name() == name)
         {
            p_file->Open();
            return;
         }
         ++vItr;
      }
      ++mItr;
   }
}

/* METHOD TO INPUT FILE INFO FROM A TEXT FILE */
void FileProcessingSystem::BuildFileSystem()
{
   ifstream fin ("FileSystem.txt");
   string buffer;
   string file_owner;
   string file_name;
   string file_path;
   int resolution[2];
   int demension[2];
   string color_space;
   int image_bit_deepth;
   int duration;
   int sound_bit_deepth;
   int file_size;
   string channel;
   int sample_freq;
   bool read_only;
   FileFormat file_format;


   // check if the file is opened
   if (!fin) {
      cout << "ERROR: Failed to open data file\n";
      exit(-1);
   }

   // keep reading until hit the end of file
   while (getline(fin, buffer, '|') )
   {
      // get file owner
      file_owner = buffer;

      // get file name
      getline (fin, buffer, '|');
      file_name = buffer;

      // get file size
      getline (fin, buffer, '|');
      file_size = atol(buffer.c_str());

      // get format
      getline (fin, buffer, '|');
      file_format = convertStrToFormat(buffer);

      // get file readonly
      getline (fin, buffer, '|');
      istringstream(buffer) >> read_only;

      // get message type specific data
      switch (file_format)
      {
         case txt: // handle text file
            getline(fin, buffer,'\n');
            file_path = buffer;

            file_map[file_owner].push_back (
               new TextFile(file_name,file_size,file_format,file_path, read_only));
            break;
         case html: // handle web file
            getline(fin, buffer,'\n');
            file_path = buffer;

            file_map[file_owner].push_back (
               new HTMLFile(file_name,file_size,file_format,file_path, read_only));
            break;
         case png: // handle image file
            getline (fin, buffer, '|');
            image_bit_deepth = atol(buffer.c_str());

            getline (fin, buffer, '|');
            istringstream(buffer,',') >> resolution[0] >> resolution[1];

            getline (fin, buffer, '|');
            istringstream(buffer,',') >> demension[0] >> demension[1];

            getline (fin, buffer, '\n');
            color_space = buffer;

            file_map[file_owner].push_back (
               new ImageFile(file_name,file_size,file_format,file_path,read_only,
                  image_bit_deepth, resolution, demension, color_space));
            break;
         case mp3: // handle song file
            getline(fin, buffer,'|');
            file_path = buffer;

            getline (fin, buffer, '|');
            duration = atol(buffer.c_str());

            getline (fin, buffer, '|');
            sound_bit_deepth = atol(buffer.c_str());

            getline (fin, buffer, '|');
            channel = buffer;

            getline (fin, buffer, '\n');
            sample_freq = atol(buffer.c_str());

            file_map[file_owner].push_back (
               new SoundFile(file_name,file_size,file_path,read_only,file_format,
                  duration, sound_bit_deepth, channel, sample_freq));
            break;
      }
   }

   // close file
   fin.close();
}

FileFormat FileProcessingSystem::convertStrToFormat(const string format) const
{
   if (format == "txt")
      return FileFormat::txt;
   else if (format == "png")
      return FileFormat::png;
   else if (format == "html")
      return FileFormat::html;
   else if (format == "mp3")
      return FileFormat::mp3;
   else 
      return FileFormat::unknown;
}