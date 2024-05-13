#include <iostream>
#include <fstream>
#include <filesystem>
#include <functional>
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

public:
    ~YoutubeVideo() {
        if (id == countOfVideos)
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
    static int getCountOfVideos() {
        return countOfVideos;
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
    friend class ThirdPartyYouTubeClass;

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
        if(id > 0)
            --countOfPlaylists;
        while (!videos.empty()) {
            delete videos.back();
            videos.pop_back();
        }
    }

    string getName() {
        return name;
    }
    int getId() {
        return id;
    }
    YoutubeVideo* getVideoById(int id) {
        for (auto video : videos)
            if (video->getId() == id)
                return video;
        return nullptr;
    }

    bool empty() {
        return videos.empty();
    }

    int size() {
        return videos.size();
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
        bool wasDeleted = false;
        YoutubeVideo* videoForDeleting;
        for (auto it = videos.begin(); it != videos.end(); ++it) {
            if ((*it)->id == id) {
                videoForDeleting = *it;
                videos.erase(it);
                delete videoForDeleting;
                wasDeleted = true;
                break;
            }
        }
        return wasDeleted;
    }

    void sortVideosById() {
        sort(videos.begin(), videos.end(), [](YoutubeVideo* a, YoutubeVideo* b) {
            return a->getId() < b->getId();
        });
    }
    void sortVideosByName() {
        sort(videos.begin(), videos.end(), [](YoutubeVideo* a, YoutubeVideo* b) {
            return a->getName() < b->getName();
        });
    }
    void sortVideosByCreator() {
        sort(videos.begin(), videos.end(), [](YoutubeVideo* a, YoutubeVideo* b) {
            return a->getCreator() < b->getCreator();
        });
    }
    void sortVideosByDate() {
        sort(videos.begin(), videos.end(), [](YoutubeVideo* a, YoutubeVideo* b) {
            return (a->getPublicationDate()->tm_year == b->getPublicationDate()->tm_year ? 
                (a->getPublicationDate()->tm_mon == b->getPublicationDate()->tm_mon ? 
                    (a->getPublicationDate()->tm_mday < b->getPublicationDate()->tm_mday) : 
                    a->getPublicationDate()->tm_mon < b->getPublicationDate()->tm_mon) : 
                a->getPublicationDate()->tm_year < b->getPublicationDate()->tm_year);
        });
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

    void printInfo() {
        cout << "Плейлист #" << id << ":\n";
        cout << "Назва плейлисту: " << name << endl;
        if (!videos.empty()) {
            cout << "Відео у складі плейлиста:\n";

            for (auto video : videos)
                cout << "Відео #" << video->getId() << ": \"" << video->getName() << "\" - " 
                << video->getCreator() << " - " << video->getStrPublicationDate() << endl;
            cout << endl;
        }
    }
    void printInfoAboutVideos() {
        for (auto video : videos)
        {
            cout << "Відео #" << video->getId() << ":\n";
            cout << "Назва: " << video->getName() << endl;
            cout << "Хто створив: " << video->getCreator() << endl;
            cout << "Дата публікації (день-місяць-рік): " << video->getStrPublicationDate() << "\n";
        }
        cout << endl;
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
        PLAYLISTS_DIRECTORY,
        VIDEOS_DIRECTORY;

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
    static string removeSpecialCharacters(string str) {
        string specialChars = "/\\\":?*|<>";
        size_t pos;
        for (char c : specialChars) {
            while ((pos = str.find(c)) != string::npos) {
                str.erase(pos, 1);
            }
        }
        return str;
    }

public:
    static void writePlaylistsData(vector<YoutubePlaylist*>* playlists) {
        deleteWastePlaylistsData(playlists);

        ofstream ofs_aval_playlists(AVAILABLE_PLAYLISTS_FILEPATH);
        for (auto it : *playlists)
            ofs_aval_playlists << it->getName() << endl;
        ofs_aval_playlists.close();

        if (!filesystem::exists(PLAYLISTS_DIRECTORY))
            filesystem::create_directories(PLAYLISTS_DIRECTORY);

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
    static void writeVideo(YoutubeVideo* video, ofstream* ofs_playlist = nullptr) {
        if(ofs_playlist)
        {
            *ofs_playlist << video->id << endl;
            *ofs_playlist << video->name << endl;
            *ofs_playlist << video->creator << endl;
            *ofs_playlist << video->getStrPublicationDate() << endl;
        }
        else {
            if (!filesystem::exists(VIDEOS_DIRECTORY))
                filesystem::create_directories(VIDEOS_DIRECTORY);

            string filename = removeSpecialCharacters(video->name);
            ofstream ofs_video(VIDEOS_DIRECTORY + filename + ".txt");
            ofs_video << "Хто створив: " << video->creator << endl;
            ofs_video << "Дата публікації: " << video->getStrPublicationDate() << endl;
            ofs_video.close();
        }
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
FilesManager::PLAYLISTS_DIRECTORY = "Playlists\\",
FilesManager::VIDEOS_DIRECTORY = "Videos\\";

class ThirdPartyYouTubeLib {
public:
    virtual YoutubePlaylist* listVideos(int id) = 0;
    virtual YoutubeVideo* getVideoInfo(int playlistId, int videoId) = 0;
    virtual bool downloadVideo(int playlistId, int videoId) = 0;

    virtual void addPlaylist(YoutubePlaylist* playlist) = 0;
    virtual void deletePlaylistById(int id) = 0;
    virtual int getCountOfPlaylists() = 0;
    virtual bool empty() = 0;
    virtual bool printListOfPlaylists() = 0;
};

class ThirdPartyYouTubeClass : public ThirdPartyYouTubeLib {
private:
    friend class Application;

    vector<YoutubePlaylist*> playlists;

    void loadData() {
        FilesManager::readPlaylists(&playlists);
    }

public:
    ThirdPartyYouTubeClass() {
        loadData();
    }
    ~ThirdPartyYouTubeClass() {
        while (!playlists.empty()) {
            delete playlists.back();
            playlists.pop_back();
        }
    }

    YoutubePlaylist* listVideos(int id) override {
        return *find_if(playlists.begin(), playlists.end(), [id](YoutubePlaylist* playlist) { return playlist->getId() == id; });
    }
    YoutubeVideo* getVideoInfo(int playlistId, int videoId) override {
        return listVideos(playlistId)->getVideoById(videoId);
    }
    bool downloadVideo(int playlistId, int videoId) override {
        auto videoToDownload = getVideoInfo(playlistId, videoId);
        if(videoToDownload)
        {
            FilesManager::writeVideo(videoToDownload);
            return true;
        }
        return false;
    }

    void addPlaylist(YoutubePlaylist* playlist) override {
        playlists.push_back(playlist);
    }
    void deletePlaylistById(int id) override {
        bool isIdOfLastElement = id == playlists.size();

        YoutubePlaylist* playlistForDeleting;
        for (auto it = playlists.begin(); it != playlists.end(); ++it) {
            if ((*it)->getId() == id) {
                playlistForDeleting = *it;
                playlists.erase(it);
                delete playlistForDeleting;
                break;
            }
        }

        if (!isIdOfLastElement)
            for (int i = 0; i < playlists.size(); ++i)
                if (playlists[i]->getId() > id)
                    playlists[i]->decrementId();
    }
    int getCountOfPlaylists() override {
        return playlists.size();
    }
    bool empty() override {
        return playlists.empty();
    }
    bool printListOfPlaylists() override {
        if (empty())
            return false;

        for (auto it : playlists)
            cout << "Плейлист #" << it->getId() << ": \"" << it->getName() << "\"\n";

        return true;
    }

    void unloadData() {
        FilesManager::writePlaylistsData(&playlists);
    }
};

class CachedYouTubeClass : public ThirdPartyYouTubeLib {
private:
    ThirdPartyYouTubeLib* service;
    YoutubePlaylist* cachedPlaylist;
    YoutubeVideo* cachedVideo;
    bool needReset;

    void resetCache() {
        cachedPlaylist = nullptr;
        cachedVideo = nullptr;
        needReset = false;
    }

public:
    CachedYouTubeClass(ThirdPartyYouTubeLib* service) {
        this->service = service;
        needReset = false;
    }
    ~CachedYouTubeClass() {
        delete service;
    }

    YoutubePlaylist* listVideos(int id) override {
        if (needReset)
            resetCache();

        if (!cachedPlaylist || cachedPlaylist->getId() != id)
            cachedPlaylist = service->listVideos(id);
        return cachedPlaylist;
    }
    YoutubeVideo* getVideoInfo(int playlistId, int videoId) override {
        if (needReset)
            resetCache();

        if (!cachedVideo || cachedVideo->getId() != videoId)
            if (!cachedPlaylist || cachedPlaylist->getId() != playlistId) {
                cachedPlaylist = service->listVideos(playlistId);
                cachedVideo = cachedPlaylist->getVideoById(videoId);
            }
            else 
                cachedVideo = cachedPlaylist->getVideoById(videoId);
        return cachedVideo;
    }
    bool downloadVideo(int playlistId, int videoId) override {
        if (needReset)
            resetCache();

        if (!cachedVideo || cachedVideo->getId() != videoId)
            if (!cachedPlaylist || cachedPlaylist->getId() != playlistId) {
                cachedPlaylist = service->listVideos(playlistId);
                cachedVideo = cachedPlaylist->getVideoById(videoId);
            }
            else 
                cachedVideo = cachedPlaylist->getVideoById(videoId);

        if(cachedVideo)
        {
            FilesManager::writeVideo(cachedVideo);
            return true;
        }
        return false;
    }
    void setNeedReset() {
        needReset = true;
    }

    void addPlaylist(YoutubePlaylist* playlist) override {
        service->addPlaylist(playlist);
    }
    void deletePlaylistById(int id) override {
        return service->deletePlaylistById(id);
    }
    int getCountOfPlaylists() override {
        return service->getCountOfPlaylists();
    }
    bool empty() override {
        return service->empty();
    }
    bool printListOfPlaylists() override {
        return service->printListOfPlaylists();
    }
};

class YoutubeManager {
private:
    ThirdPartyYouTubeLib* service;

    void printSetOfVideos(string msg, vector<YoutubeVideo*>* videos) {
        cout << msg;

        for (auto video : *videos)
            cout << "Відео #" << video->getId() << ": \"" << video->getName() << "\" - "
            << video->getCreator() << " - " << video->getStrPublicationDate() << endl;
        cout << endl;
    }

public:
    YoutubeManager(ThirdPartyYouTubeLib* service) {
        this->service = service;        
    }
    ~YoutubeManager() {
        delete service;
    }

    bool createVideo(int playlistId) {
        YoutubeVideo* video = new YoutubeVideo();
        if (!video->enterInfo()) {
            delete video;
            return false;
        }
        service->listVideos(playlistId)->addVideo(video);

        return true;
    }
    bool createPlaylist() {
        YoutubePlaylist* playlist = new YoutubePlaylist();
        if (!playlist->enterInfo()) {
            delete playlist;
            return false;
        }
        service->addPlaylist(playlist);

        return true;
    }

    bool deleteVideo(int playlistId, int videoId) {
        return service->listVideos(playlistId)->deleteVideoById(videoId);
    }
    void deletePlaylist(int id) {
        service->deletePlaylistById(id);
    }

    bool downloadVideo(int playlistId, int videoId) {
        return service->downloadVideo(playlistId, videoId);
    }

    void resetCache() {
        dynamic_cast<CachedYouTubeClass*>(service)->setNeedReset();
    }

    void printInfoAboutPlaylist(int id) {
        service->listVideos(id)->printInfo();
    }
    void printInfoAboutVideosFromPlaylist(int id) {
        service->listVideos(id)->printInfoAboutVideos();
    }
    bool printListOfPlaylists() {
        return service->printListOfPlaylists();
    }

    int getCountOfPlaylists() {
        return service->getCountOfPlaylists();
    }
    YoutubePlaylist* getPlaylist(int id) {
        return service->listVideos(id);
    }
    YoutubeVideo* getVideo(int playlistId, int videoId) {
        return service->getVideoInfo(playlistId, videoId);
    }

    bool doesAnyPlaylistExist() {
        return !service->empty();
    }    

    void sortPlaylistById(int id) {
        service->listVideos(id)->sortVideosById();
    }
    void sortPlaylistByName(int id) {
        service->listVideos(id)->sortVideosByName();
    }
    void sortPlaylistByCreator(int id) {
        service->listVideos(id)->sortVideosByCreator();
    }
    void sortPlaylistByDate(int id) {
        service->listVideos(id)->sortVideosByDate();
    }

    vector<YoutubeVideo*> findVideosByWordInNames(int playlistId, string word) {
        auto videosByName = service->listVideos(playlistId)->findVideosByWordInNames(word);
        if (videosByName.empty())
            return videosByName;
        printSetOfVideos("Відео, які в назві містять слово \"" + word + "\":\n", &videosByName);
        return videosByName;
    }
    vector<YoutubeVideo*> findVideosByWordInCreators(int playlistId, string word) {
        auto videosByCreator = service->listVideos(playlistId)->findVideosByWordInCreators(word);
        if (videosByCreator.empty())
            return videosByCreator;
        printSetOfVideos("Відео, канали яких в назві містять слово \"" + word + "\":\n", &videosByCreator);
        return videosByCreator;
    }
    vector<YoutubeVideo*> findVideosByDateRange(int playlistId, string startDateStr, string endDateStr) {
        auto videosByDateRange = service->listVideos(playlistId)->findVideosByDateRange(startDateStr, endDateStr);
        if (videosByDateRange.empty())
            return videosByDateRange;
        printSetOfVideos("Відео, дата публікації яких входить до діапазону " + startDateStr + "-" + endDateStr + ":\n", &videosByDateRange);
        return videosByDateRange;
    }
};

class Application {
private:
    int currentPlaylistId;
    YoutubeManager* manager;
    function<void()> unloadDataFunction;

    void setConsoleColor(int colorCode) {
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(hConsole, colorCode);
    }

    void pauseAndCleanConsole() {
        system("pause");
        system("cls");
    }

    void successNotification(const char* msg) {
        cout << "\nУспіх: " << msg << "\n\n";
    }

    void errorNotification(const char* msg) {
        cout << "\nПомилка: " << msg << "\n\n";
    }

    void printNotification(const char* type, const char* msg) {
        if (strcmp(type, "success") == 0) {
            setConsoleColor(10); // Green color
            successNotification(msg);
        }
        else {
            setConsoleColor(12); // Red color
            errorNotification(msg);
        }
        setConsoleColor(15); // White color
        pauseAndCleanConsole();
    }
    bool validateEnteredNumber(string option, short firstOption, short lastOption) {
        if (option.empty())
            return false;

        for (char num : option)
            if (num < '0' || num > '9')
                return false;

        auto convertedValue = stoull(option);

        return firstOption <= convertedValue && convertedValue <= lastOption;
    }
    int enterNumberInRange(string message, int firstOption, int lastOption) {
        bool isOptionVerified = false;
        string option;

        cout << "\n" << message;
        getline(cin, option);

        isOptionVerified = validateEnteredNumber(option, firstOption, lastOption);

        if (!isOptionVerified)
            printNotification("error", "неправильній вибір! Спробуйте знов!");

        return isOptionVerified ? stoi(option) : -1;
    }

    void printManagingSessionsMenu(int& choice) {
        cout << "Головне меню:\n";
        cout << "0. Закрити програму\n";
        cout << "1. Створити плейлист\n";
        cout << "2. Відкрити плейлист\n";
        cout << "3. Видалити плейлист\n";
        choice = enterNumberInRange("Ваш вибір: ", 0, 3);
    }

    void createPlaylist() {
        if (manager->createPlaylist())
            printNotification("success", "плейлист був успішно створений!");
        else
            printNotification("error", "назва може складатись тільки з літер, цифр та пробілів!");
    }

    bool doesAnyPlaylistExist() {
        if(!manager->doesAnyPlaylistExist())
            printNotification("error", "в даний момент жодного плейлиста немає!");
        return manager->doesAnyPlaylistExist();
    }

    void printListOfAvailablePlaylists() {
        if(!manager->printListOfPlaylists())
            printNotification("error", "в даний момент жодного плейлиста немає!");
    }

    void addVideoToPlaylist() {
        if (manager->createVideo(currentPlaylistId))
            printNotification("success", "відео було успішно додано до плейлиста!");
        else
            printNotification("error", "неправильно введені дані!\nНазва відео повинна містити хоча б один символ,\n назва каналу може складатись тільки з літер, цифр та пробілів,\nа дата повинна бути введена у форматі!");
    }

    bool doesPlaylistHaveAnyVideo() {
        if (manager->getPlaylist(currentPlaylistId)->empty())
            printNotification("error", "в даному плейлисті немає жодного відео!");
        return !manager->getPlaylist(currentPlaylistId)->empty();
    }

    void findVideosByNames() {
        string word;
        cout << "\nВведіть підрядок, за яким буде відбуватись пошук: ";
        getline(cin, word);
        if(word.empty())
        {
            printNotification("error", "потрібно ввести підрядок!");
            return;
        }
        cout << endl;
        auto foundVideos = manager->findVideosByWordInNames(currentPlaylistId, word);
        if (foundVideos.empty())
            printNotification("error", "жодне відео цього плейлиста в назві не містить уведене слово!");
        else
            system("pause");
    }

    void findVideosByCreators() {
        string word;
        cout << "\nВведіть підрядок, за яким буде відбуватись пошук: ";
        getline(cin, word);
        if (word.empty())
        {
            printNotification("error", "потрібно ввести підрядок!");
            return;
        }
        cout << endl;
        auto foundVideos = manager->findVideosByWordInCreators(currentPlaylistId, word);
        if (foundVideos.empty())
            printNotification("error", "жодне відео цього плейлиста в назві каналу не містить уведене слово!");
        else
            system("pause");
    }

    void findVideosByDates() {
        string startDate, endDate;
        cout << "\nВведіть початкову дату (день-місяць-рік): ";
        getline(cin, startDate);
        cout << "Введіть кінцеву дату (день-місяць-рік): ";
        getline(cin, endDate);
        cout << endl;
        auto foundVideos = manager->findVideosByDateRange(currentPlaylistId, startDate, endDate);
        if (foundVideos.empty())
            printNotification("error", "або дати введені не в правильному форматі, або відсутні відео, опубліковані в цьому проміжку дат!");
        else
            system("pause");
    }

    void workWithPlaylistMenu(int& choice) {
        cout << "Меню дій над плейлистом " << manager->getPlaylist(currentPlaylistId)->getName() << ":\n";
        cout << "0. Назад\n";
        cout << "1. Додати відео\n";
        cout << "2. Завантажити на ПК відео\n";
        cout << "3. Видалити відео\n\n";
        cout << "4. Вивести інформацію про усі відео\n";
        cout << "5. Вивести інформацію про певне відео\n\n";
        cout << "6. Відсортувати відео за ID\n";
        cout << "7. Відсортувати відео за назвою\n";
        cout << "8. Відсортувати відео за назвою каналу\n";
        cout << "9. Відсортувати відео за датою\n\n";
        cout << "10. Пошук відео за словом в назві\n";
        cout << "11. Пошук відео за словом в назві каналу\n";
        cout << "12. Пошук відео з певної по певну дату\n\n";
        cout << "13. Очистити кеш програми\n";
        choice = enterNumberInRange("Ваш вибір: ", 0, 13);
    }

    void printInfoAboutVideo() {
        int id = enterNumberInRange("Введіть ID відео: ", 1, YoutubeVideo::getCountOfVideos());
        if (id == -1)
            return;
        auto videoToPrint = manager->getVideo(currentPlaylistId, id);
        if (videoToPrint)
        {
            videoToPrint->printInfo();
            system("pause");
        }
        else
            printNotification("error", "в даному плейлисті немає відео з таким ID!");
    }

    void downloadVideo() {
        int id = enterNumberInRange("Введіть ID відео: ", 1, YoutubeVideo::getCountOfVideos());
        if (id == -1)
            return;
        if(manager->downloadVideo(currentPlaylistId, id))
            printNotification("success", "відео було успішно завантажене!");
        else
            printNotification("error", "в даному плейлисті немає відео з таким ID!");
    }

    void deleteVideo() {
        int id = enterNumberInRange("Введіть ID відео: ", 1, YoutubeVideo::getCountOfVideos());
        if (id == -1)
            return;
        if(manager->deleteVideo(currentPlaylistId, id))
            printNotification("success", "відео було успішно видалено!");
        else
            printNotification("error", "в даному плейлисті немає відео з таким ID!");
    }

    void executeWorkWithPlaylistMenu() {
        int choice;

        do
        {
            system("cls");
            manager->printInfoAboutPlaylist(currentPlaylistId);
            workWithPlaylistMenu(choice);
            switch (choice)
            {
            case 0:
                cout << "\nПовернення до головного меню.\n";
                return;
            case 1:
                addVideoToPlaylist();
                continue;
            case 2:
            case 3:
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
            case 9:
            case 10:
            case 11:
            case 12:
                if(doesPlaylistHaveAnyVideo())
                    switch (choice)
                    {
                    case 2:
                        downloadVideo();
                        continue;
                    case 3:
                        deleteVideo();
                        continue;
                    case 4:
                        manager->printInfoAboutVideosFromPlaylist(currentPlaylistId);
                        system("pause");
                        continue;
                    case 5:
                        printInfoAboutVideo();
                        continue;
                    case 6:
                        manager->sortPlaylistById(currentPlaylistId);
                        printNotification("success", "дані були успішно відсортовані за ID!");
                        continue;
                    case 7:
                        manager->sortPlaylistByName(currentPlaylistId);
                        printNotification("success", "дані були успішно відсортовані за назвою!");
                        continue;
                    case 8:
                        manager->sortPlaylistByCreator(currentPlaylistId);
                        printNotification("success", "дані були успішно відсортовані за назвою канала!");
                        continue;
                    case 9:
                        manager->sortPlaylistByDate(currentPlaylistId);
                        printNotification("success", "дані були успішно відсортовані за датою!");
                        continue;
                    case 10:
                        findVideosByNames();
                        continue;
                    case 11:
                        findVideosByCreators();
                        continue;
                    case 12:
                        findVideosByDates();
                        continue;
                    }
                continue;
            case 13:
                manager->resetCache();
                printNotification("success", "кеш програми був успішно очищений!");
            }
        } while (true);
    }

    void deleteOrOpenPlaylist(string action) {
        int id = enterNumberInRange("Введіть номер плейлиста: ", 1, manager->getCountOfPlaylists());
        if (id == -1)
            return;
        if (action == "видалити")
        {
            manager->deletePlaylist(id);
            printNotification("success", "плейлист був успішно видалений!");
        }
        else {
            currentPlaylistId = id;
            executeWorkWithPlaylistMenu();
        }
    }
public:
    Application() {
        auto youTubeService = new ThirdPartyYouTubeClass();
        unloadDataFunction = [=]() {
            youTubeService->unloadData();
            };
        auto youTubeProxy = new CachedYouTubeClass(youTubeService);
        manager = new YoutubeManager(youTubeProxy);
    }
    ~Application() {
        delete manager;
    }
    
    void executeMainMenu() {
        int choice;
        
        do
        {
            system("cls");
            printListOfAvailablePlaylists();
            printManagingSessionsMenu(choice); 
            switch (choice)
            {
            case 0:
                cout << "\nДо побачення!\n";
                unloadDataFunction();
                return;
            case 1:
                createPlaylist();
                continue;
            case 2:
            case 3:
                if (doesAnyPlaylistExist())
                    choice == 2 ? deleteOrOpenPlaylist("відкрити") :
                    deleteOrOpenPlaylist("видалити");
            }
        } while (true);
    }
};

int main()
{
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    Application* app = new Application();
    app->executeMainMenu();
    delete app;
}