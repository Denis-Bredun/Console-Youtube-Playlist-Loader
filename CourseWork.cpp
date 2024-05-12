#include <iostream>
#include <fstream>
#include <filesystem>
#include <time.h>
#include <algorithm>
#include <vector>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <windows.h>
using namespace std;

class Validator {
private:
    static int countHyphens(string str) {
        return count(str.begin(), str.end(), '-');
    }
    static bool isOnlySpaces(string str) {
        return all_of(str.begin(), str.end(), [](char c) {
            return isspace(static_cast<unsigned char>(c));
        });
    }

public:   
    static bool isNameValid(string vname, int maxSize) {
        return !(vname.empty() || isOnlySpaces(vname) || vname.size() > maxSize);
    }    
    static bool isDateValid(string str, struct tm* tempDate) {
        if (!(str.empty() || countHyphens(str) != 2)) {
            stringstream ss(str);

            ss >> get_time(tempDate, "%d-%m-%Y");

            if (ss.fail()) {
                delete tempDate;
                return false;
            }
            return true;
        }
        return false;
    }
    static bool isDateInRange(struct tm* date, struct tm* startDate, struct tm* endDate) {
        int entryDateCalc = (date->tm_year * 10000) + (date->tm_mon * 100) + date->tm_mday;
        int startDateCalc = (startDate->tm_year * 10000) + (startDate->tm_mon * 100) + startDate->tm_mday;
        int endDateCalc = (endDate->tm_year * 10000) + (endDate->tm_mon * 100) + endDate->tm_mday;

        return entryDateCalc >= startDateCalc && entryDateCalc <= endDateCalc ? true : false;
    }
    static bool containsOnlyLettersNumbersSpaces(string str) {
        return all_of(str.begin(), str.end(), [](char c) {
            return isalnum(c) || c == ' ';
        });
    }
};

class YoutubeVideo {
private:
    friend class FilesManager;
    friend class YoutubePlaylist;

    string name, creator;
    struct tm* publicationDate;
    int id;

    static int countOfVideos;

    void setId() {
        ++countOfVideos;
        id = countOfVideos;
    }
    void decrementId() {
        --id;
    }

public:
    ~YoutubeVideo() {
        --countOfVideos;
        if (!publicationDate)
            delete publicationDate;
    }

    int getId() {
        return id;
    }
    string getName() {
        return name;
    }
    string getCreator() {
        return creator;
    }
    string getStrPublicationDate() {
        char str[20];
        strftime(str, sizeof(str), "%d-%m-%Y", publicationDate);
        return str;
    }
    struct tm* getPublicationDate() {
        return publicationDate;
    }

    bool setName(string name) {
        if (!Validator::isNameValid(name, 70))
            return false;

        this->name = name;
        return true;
    }
    bool setCreator(string creator) {
        if (!Validator::isNameValid(creator, 36) || !Validator::containsOnlyLettersNumbersSpaces(creator))
            return false;

        this->creator = creator;
        return true;
    }
    bool setPublicationDate(string date) {
        struct tm* tempDate = new tm;
        if (!Validator::isDateValid(date, tempDate))
            return false;

        if (publicationDate)
            delete publicationDate;

        publicationDate = tempDate;

        return true;
    }

    void printInfo()
    {
        cout << "Відео #" << id << ":\n";
        cout << "Назва: " << name << endl;
        cout << "Хто створив: " << creator << endl;
        cout << "Дата публікації (день-місяць-рік): " << getStrPublicationDate() << "\n\n";
    }
    bool enterInfo() {
        string data;
        cout << "Введіть назву відео: ";
        getline(cin, data);
        if (!setName(data)) return false;

        cout << "Введіть назву каналу: ";
        getline(cin, data);
        if (!setCreator(data)) return false;

        cout << "Введіть дату публікації (день-місяць-рік): ";
        getline(cin, data);
        if (!setPublicationDate(data)) return false;

        setId();

        return true;
    }
};

int YoutubeVideo::countOfVideos = 0;

class YoutubePlaylist {
private:
    friend class FilesManager;

    vector<YoutubeVideo*> videos;
    string name;
    int id;

    static int countOfPlaylists;

    void setId() {
        ++countOfPlaylists;
        id = countOfPlaylists;
    }
    void decrementId() {
        --id;
    }

public:
    ~YoutubePlaylist() {
        --countOfPlaylists;
        while (!videos.empty()) {
            delete videos.back();
            videos.pop_back();
        }
    }

    string getName() {
        return name;
    }
    YoutubeVideo* getVideoById(int id) {
        return *find_if(videos.begin(), videos.end(), [id](YoutubeVideo* video) {return video->getId() == id; });
    }

    bool setName(string name) {
        if (!Validator::isNameValid(name, 70) || !Validator::containsOnlyLettersNumbersSpaces(name))
            return false;

        this->name = name;
        return true;
    }

    void addVideo(YoutubeVideo* video) {
        videos.push_back(video);
    }
    bool deleteVideoById(int id) {
        if (videos.empty())
            return false;

        bool isIdOfLastElement = id == videos.size();

        YoutubeVideo* videoForDeleting;
        for (auto it = videos.begin(); it != videos.end(); ++it) {
            if ((*it)->id == id) {
                videoForDeleting = *it;
                videos.erase(it);
                delete videoForDeleting;
                break;
            }
        }
        
        if(!isIdOfLastElement) 
            for (int i = 0; i < videos.size(); ++i)
                if(videos[i]->getId() > id)
                    videos[i]->decrementId();

        return true;
    }

    bool sortVideosById() {
        if (videos.empty())
            return false;

        sort(videos.begin(), videos.end(), [](YoutubeVideo* a, YoutubeVideo* b) {
            return a->getId() < b->getId();
        });

        return true;
    }
    bool sortVideosByName() {
        if (videos.empty())
            return false;

        sort(videos.begin(), videos.end(), [](YoutubeVideo* a, YoutubeVideo* b) {
            return a->getName() < b->getName();
        });

        return true;
    }
    bool sortVideosByCreator() {
        if (videos.empty())
            return false;

        sort(videos.begin(), videos.end(), [](YoutubeVideo* a, YoutubeVideo* b) {
            return a->getCreator() < b->getCreator();
        });

        return true;
    }
    bool sortByDate() {
        if (videos.empty())
            return false;

        sort(videos.begin(), videos.end(), [](YoutubeVideo* a, YoutubeVideo* b) {
            return (a->getPublicationDate()->tm_year == b->getPublicationDate()->tm_year ? 
                (a->getPublicationDate()->tm_mon == b->getPublicationDate()->tm_mon ? 
                    (a->getPublicationDate()->tm_mday < b->getPublicationDate()->tm_mday) : 
                    a->getPublicationDate()->tm_mon < b->getPublicationDate()->tm_mon) : 
                a->getPublicationDate()->tm_year < b->getPublicationDate()->tm_year);
        });

        return true;
    }

    vector<YoutubeVideo*> findVideosByWordInNames(string word) {
        vector<YoutubeVideo*> videosByName;

        for (auto video : this->videos) {
            if (video->getName().find(word) != string::npos)
                videosByName.push_back(video);
        }

        return videosByName;
    }
    vector<YoutubeVideo*> findVideosByWordInCreators(string word) {
        vector<YoutubeVideo*> videosByCreator;

        for (auto video : this->videos) {
            if (video->getCreator().find(word) != string::npos)
                videosByCreator.push_back(video);
        }

        return videosByCreator;
    }
    vector<YoutubeVideo*> findVideosByDateRange(string startDateStr, string endDateStr) {
        vector<YoutubeVideo*> videosByDateRange;

        struct tm* startDate = new tm, * endDate = new tm;
        if (!Validator::isDateValid(startDateStr, startDate))
        {
            delete endDate;
            return videosByDateRange;
        }
        if(!Validator::isDateValid(endDateStr, endDate))
        {
            delete startDate;
            return videosByDateRange;
        }

        for (auto video : this->videos) {
            if (Validator::isDateInRange(video->getPublicationDate(), startDate, endDate))
                videosByDateRange.push_back(video);
        }

        return videosByDateRange;
    }

    bool printInfo() {
        cout << "Плейлист #" << id << ":\n";
        cout << "Назва плейлисту: " << name << endl;
        if (!videos.empty()) {
            cout << "Відео у складі плейлиста:\n";

            for (auto video : videos)
                cout << "Відео #" << video->getId() << ": \"" << video->getName() << "\" - " 
                << video->getCreator() << " - " << video->getStrPublicationDate() << endl;
            cout << endl;

            return true;
        }
        else
            return false;
    }
    bool enterInfo() {
        string data;
        cout << "Введіть назву плейлисту: ";
        getline(cin, data);
        if (!setName(data)) return false;

        setId();

        return true;
    }
};

int YoutubePlaylist::countOfPlaylists = 0;

class FilesManager {
private:
    static const string AVAILABLE_PLAYLISTS_FILEPATH,
        PLAYLISTS_DIRECTORY;

    static vector<string> getFilepathesForPlaylists() {
        vector<string> filepathesForPlaylists;

        auto iteratorOnFiles = filesystem::directory_iterator(PLAYLISTS_DIRECTORY);

        for (const auto& entry : iteratorOnFiles)
            filepathesForPlaylists.push_back(entry.path().string());

        return filepathesForPlaylists;
    }
    static void deleteWastePlaylistsData(vector<YoutubePlaylist*>* playlists) {
        if (!filesystem::exists(PLAYLISTS_DIRECTORY))
            return;

        auto filepathesForPlaylists = getFilepathesForPlaylists();
        string playlistFilepath;
        bool isPartOfRealtimePlaylist;

        for (int i = 0; i < filepathesForPlaylists.size(); i++)
        {
            isPartOfRealtimePlaylist = false;
            for (int j = 0; j < playlists->size(); j++)
            {
                playlistFilepath = PLAYLISTS_DIRECTORY + (*playlists)[j]->getName();
                if (filepathesForPlaylists[i] == playlistFilepath)
                    isPartOfRealtimePlaylist = true;
            }

            if (!isPartOfRealtimePlaylist)
                remove(filepathesForPlaylists[i].c_str());
        }
    }

public:
    static void writePlaylistsData(vector<YoutubePlaylist*>* playlists) {
        deleteWastePlaylistsData(playlists);

        ofstream ofs_aval_playlists(AVAILABLE_PLAYLISTS_FILEPATH);
        for (auto it : *playlists)
            ofs_aval_playlists << it->getName() << endl;
        ofs_aval_playlists.close();

        if (!std::filesystem::exists(PLAYLISTS_DIRECTORY))
            std::filesystem::create_directories(PLAYLISTS_DIRECTORY);

        for (auto playlist : *playlists)
            writePlaylistData(playlist);
    }

    static void writePlaylistData(YoutubePlaylist* playlist) {
        ofstream ofs_playlist(PLAYLISTS_DIRECTORY + playlist->name + ".txt");

        ofs_playlist << playlist->name << endl;
        ofs_playlist << playlist->id << endl;
        ofs_playlist << playlist->countOfPlaylists << endl;

        ofs_playlist << playlist->videos.size() << endl;

        ofs_playlist << YoutubeVideo::countOfVideos << endl;

        for (int i = 0; i < playlist->videos.size(); ++i) 
            writeVideo(playlist->videos[i], &ofs_playlist);

        ofs_playlist.close();
    }

    static void writeVideo(YoutubeVideo* video, ofstream* playlist) {
        *playlist << video->id << endl;
        *playlist << video->name << endl;
        *playlist << video->creator << endl;
        *playlist << video->getStrPublicationDate() << endl;
    }

    static vector<string> readNamesOfAvailablePlaylists() {
        vector<string> aval_playlists;

        ifstream ifs_aval_playlists(AVAILABLE_PLAYLISTS_FILEPATH);

        if (!ifs_aval_playlists.is_open())
            return aval_playlists;

        string line;

        while (getline(ifs_aval_playlists, line))
            aval_playlists.push_back(line);

        return aval_playlists;
    }

    static void readPlaylists(vector<YoutubePlaylist*>* playlists) {
        if (!filesystem::exists(PLAYLISTS_DIRECTORY))
            return;

        auto namesOfPlaylists = readNamesOfAvailablePlaylists();

        for (auto name : namesOfPlaylists) {
            ifstream ifs_playlist(PLAYLISTS_DIRECTORY + name + ".txt");
            readPlaylist(playlists, &ifs_playlist);
            ifs_playlist.close();
        }
    }

    static void readPlaylist(vector<YoutubePlaylist*>* playlists, ifstream* ifs_playlist) {
        string line;
        YoutubePlaylist* playlist = new YoutubePlaylist();
        int countOfVideos;

        getline(*ifs_playlist, line);
        playlist->setName(line);

        getline(*ifs_playlist, line);
        playlist->id = stoi(line);

        getline(*ifs_playlist, line);
        playlist->countOfPlaylists = stoi(line);

        getline(*ifs_playlist, line);
        int countOfVideosInThisPlaylist = stoi(line);

        getline(*ifs_playlist, line);
        YoutubeVideo::countOfVideos = stoi(line);

        if (countOfVideosInThisPlaylist == 0) {
            playlists->push_back(playlist);
            return;
        }
        else {
            for (int i = 0; i < countOfVideosInThisPlaylist; i++)
                readVideo(playlist, ifs_playlist);
            playlists->push_back(playlist);
        }
    }

    static void readVideo(YoutubePlaylist* playlist, ifstream* ifs_playlist) {
        YoutubeVideo* video = new YoutubeVideo();
        string line;

        getline(*ifs_playlist, line);
        video->id = stoi(line);

        getline(*ifs_playlist, line);
        video->setName(line);

        getline(*ifs_playlist, line);
        video->setCreator(line);

        getline(*ifs_playlist, line);
        video->setPublicationDate(line);

        playlist->addVideo(video);
    }
};

const string FilesManager::AVAILABLE_PLAYLISTS_FILEPATH = "Available playlists.txt",
FilesManager::PLAYLISTS_DIRECTORY = "Playlists\\";















int main()
{
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    vector<YoutubePlaylist*> playlists;
    FilesManager::readPlaylists(&playlists);

    YoutubePlaylist* pl = new YoutubePlaylist();
    pl->enterInfo();
    YoutubePlaylist* pl2 = new YoutubePlaylist();
    pl2->enterInfo();
    YoutubePlaylist* pl3 = new YoutubePlaylist();
    pl3->enterInfo();
    playlists.push_back(pl);
    playlists.push_back(pl2);
    playlists.push_back(pl3);
    FilesManager::writePlaylistsData(&playlists);
















































    //vector<YoutubeVideo*> videos(10);
    //
    //cout << "Введення даних:\n";
    //for (int i = 0; i < videos.size(); ++i)
    //{
    //    videos[i] = new YoutubeVideo();
    //    videos[i]->enterInfo();
    //    cout << "\n";
    //}
    
    //cout << "\nВивід інформації:\n";
    //for (int i = 0; i < videos.size(); ++i)
    //    videos[i]->printInfo();
    
   //cout << "\nСтворення плейлиста, ввід-вивід даних:\n";
   //YoutubePlaylist* playlist1 = new YoutubePlaylist();
   //YoutubePlaylist* playlist2 = new YoutubePlaylist();
   //playlist1->enterInfo();
   //playlist2->enterInfo();
   //
   //for (int i = 0; i < 5; ++i)
   //    playlist1->addVideo(videos[i]);
   //
   //for (int i = 5; i < 10; ++i)
   //    playlist2->addVideo(videos[i]);

    //vector<YoutubePlaylist*> playlistsToRead, playlistsToRead2;
    //playlists.push_back(playlist1);
    //playlists.push_back(playlist2);
    //
    //FilesManager::writePlaylistsData(&playlists);
    //
    //playlist2->deleteVideoById(8);
    //playlist2->deleteVideoById(9);
    //
    //FilesManager::writePlaylistsData(&playlists);

    //FilesManager::readPlaylists(&playlistsToRead);
    //FilesManager::writePlaylistsData(&playlistsToRead);
    //playlistsToRead.pop_back();

    //cout << "\nДодавання відео в плейлист...\n";
    //for (auto video : videos)
    //    playlist->addVideo(video);
    
    //cout << "\nВивід даних плейлиста:\n";
    //playlist->printInfo();
    //
    //cout << "\nСортування за іменем:\n";
    //playlist->sortVideosByName();
    //playlist->printInfo();
    //
    //cout << "\nСортування за датою:\n";
    //playlist->sortByDate();
    //playlist->printInfo();
    //
    //cout << "\nСортування за creator:\n";
    //playlist->sortVideosByCreator();
    //playlist->printInfo();
    //
    
    //
    //cout << "\nПошук за іменем:\n";
    //string word;
    //cin >> word;
    //auto videosByName = playlist->findVideosByWordInNames(word);
    //for (auto video:videosByName)
    //    video->printInfo();
    //
    //cout << "\nПошук за creator:\n";
    //cin >> word;
    //auto videosByCreator = playlist->findVideosByWordInCreators(word);
    //for (auto video : videosByCreator)
    //    video->printInfo();
    
    //cout << "\nПошук за датами:\n";
    //string startDate, endDate;
    //cin >> startDate;
    //cin >> endDate;
    //auto videosByDate = playlist->findVideosByDateRange(startDate, endDate);
    //for (auto video : videosByDate)
    //    video->printInfo();
    //
    //cout << "\nСортування за id:\n";
    //playlist->sortVideosById();
    //playlist->printInfo();





    //cout << "\nВидалення елементів:\n";
    //cout << "\n6-ого:\n";
    //playlist->deleteVideoById(6);
    //playlist->printInfo();
    //cout << "\n1-ого:\n";
    //playlist->deleteVideoById(1);
    //playlist->printInfo();
    //cout << "\n3-ого:\n";
    //playlist->deleteVideoById(3);
    //playlist->printInfo();
    
    //cout << "\nСортування за іменем:\n";
    //playlist->sortVideosByName();
    //playlist->printInfo();
    //
    //cout << "\nВидалення елементів:\n";
    //cout << "\n6-ого:\n";
    //playlist->deleteVideoById(6);
    //playlist->printInfo();
    //cout << "\n3-ого:\n";
    //playlist->deleteVideoById(3);
    //playlist->printInfo();
    //cout << "\n1-ого:\n";
    //playlist->deleteVideoById(1);
    //playlist->printInfo();
}