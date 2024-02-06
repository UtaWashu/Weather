#include <iostream>
#include <string>
#include <vector>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;

// Структура для хранения данных о погоде на конкретный день
struct DailyWeatherForecast {
    string date;
    double temperature;
    string description;
};

// Функция обратного вызова для записи полученного ответа от сервера в переменную
size_t writeCallback(char* ptr, size_t size, size_t nmemb, string* data) {
    data->append(ptr, size * nmemb);
    return size * nmemb;
}

// Функция для выполнения запроса к API openweathermap.org и получения прогноза погоды
json getWeatherForecast(const string& city) {
    string apiKey = "5ae2ce314387f8118ec73263ed1ff985";
    string url = "http://api.openweathermap.org/data/2.5/forecast?q=" + city + "&appid=" + apiKey;

    CURL* curl = curl_easy_init();
    string response;

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        CURLcode res = curl_easy_perform(curl);

        if (res != CURLE_OK)
            cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << endl;

        curl_easy_cleanup(curl);
    }

    return json::parse(response);
}

// Функция для заполнения списка объектов DailyWeatherForecast из полученного прогноза погоды
vector<DailyWeatherForecast> fillWeatherForecast(const json& forecastData) {
    vector<DailyWeatherForecast> weatherForecast;

    for (const auto& item : forecastData["list"]) {
        DailyWeatherForecast dailyForecast;

        dailyForecast.date = item["dt_txt"];
        dailyForecast.temperature = item["main"]["temp"];
        dailyForecast.description = item["weather"][0]["description"];

        weatherForecast.push_back(dailyForecast);
    }

    return weatherForecast;
}

// Функция для отображения прогноза погоды на все 5 дней
void showWeatherForecast(const vector<DailyWeatherForecast>& weatherForecast) {
    for (const auto& forecast : weatherForecast) {
        cout << "День: " << forecast.date.substr(0, 10) << endl;
        cout << "Время: " << forecast.date.substr(11, 8) << endl;
        cout << "Температура: " << forecast.temperature << " K" << endl;
        cout << "Описание: " << forecast.description << endl;
        cout << endl;
    }
}

// Функция для отображения прогноза погоды на конкретный день
void showSpecificDayWeatherForecast(const vector<DailyWeatherForecast>& weatherForecast) {
    cout << "Введите дату прогноза (yyyy-mm-dd): ";
    string date;
    cin >> date;

    bool isWeatherAvailable = false;

    for (const auto& forecast : weatherForecast) {
        if (forecast.date.substr(0, 10) == date) {
            isWeatherAvailable = true;

            cout << "День: " << forecast.date.substr(0, 10) << endl;
            cout << "Температура: " << forecast.temperature << " K" << endl;
            cout << "Описание: " << forecast.description << endl;
            break;
        }
    }

    if (!isWeatherAvailable)
        cout << "Прогноз погоды на указанную дату отсутствует." << endl;
}

// Функция для отображения самой низкой и самой высокой температур из всех прогнозов
void showTemperatureExtremes(const vector<DailyWeatherForecast>& weatherForecast) {
    double minTemperature = weatherForecast[0].temperature;
    double maxTemperature = weatherForecast[0].temperature;

    for (const auto& forecast : weatherForecast) {
        minTemperature = min(minTemperature, forecast.temperature);
        maxTemperature = max(maxTemperature, forecast.temperature);
    }

    cout << "Самая низкая температура: " << minTemperature << " K" << endl;
    cout << "Самая высокая температура: " << maxTemperature << " K" << endl;
}


int main() {
    string city;
    cout << "Введите название города: ";
    cin >> city;

    json forecastData = getWeatherForecast(city);
    vector<DailyWeatherForecast> weatherForecast = fillWeatherForecast(forecastData);

    int choice = 0;
while (choice != 4) {
        cout << endl;
        cout << "Меню:" << endl;
        cout << "1. Просмотр прогноза на 5 дней" << endl;
        cout << "2. Просмотр прогноза на конкретный день" << endl;
        cout << "3. Отображение самой низкой и самой высокой температур" << endl;
        cout << "4. Выход" << endl;

        cout << "Выберите действие: ";
        cin >> choice;

        switch (choice) {
            case 1:
                showWeatherForecast(weatherForecast);
                break;
            case 2:
                showSpecificDayWeatherForecast(weatherForecast);
                break;
            case 3:
                showTemperatureExtremes(weatherForecast);
                break;
            case 4:
                cout << "Выход" << endl;
                break;
            default:
                cout << "Выбрано неверное действие. Попробуйте снова." << endl;
                break;
        }

        cout << endl;
    }

    return 0;
}
