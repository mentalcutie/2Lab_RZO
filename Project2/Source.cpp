#include <opencv2/opencv.hpp>
#include <iostream>

int main() {
    // Загрузка изображения
    cv::Mat src = cv::imread("shapes.jpg");
    if (src.empty()) {
        std::cerr << "Ошибка загрузки изображения!" << std::endl;
        return -1;
    }

    // Преобразование в оттенки серого
    cv::Mat gray;
    cv::cvtColor(src, gray, cv::COLOR_BGR2GRAY);

    // Усиленное сглаживание
    cv::Mat blurred;
    cv::GaussianBlur(gray, blurred, cv::Size(1, 1), 2.0);

    // Обнаружение краёв
    cv::Mat edges;
    cv::Canny(blurred, edges, 20, 120);

    // Морфологическое закрытие для сглаживания
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));
    cv::morphologyEx(edges, edges, cv::MORPH_CLOSE, kernel);

    // Поиск контуров
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(edges, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    // Создание выходного изображения
    cv::Mat output = cv::Mat::zeros(src.size(), src.type());

    // Обработка контуров
    for (size_t i = 0; i < contours.size(); i++) {
        double area = cv::contourArea(contours[i]);
        if (area > 200) { // Фильтрация мелких шумов
            std::vector<cv::Point> approx;
            cv::approxPolyDP(contours[i], approx, 0.02 * cv::arcLength(contours[i], true), true);

            std::string shapeName;
            if (approx.size() == 3) shapeName = "triangle";
            else if (approx.size() == 4) shapeName = "square";
            else if (approx.size() > 4) shapeName = "circle";
            else continue;

            cv::Moments m = cv::moments(contours[i]);
            cv::Point center(m.m10 / m.m00, m.m01 / m.m00);

            cv::drawContours(output, contours, (int)i, cv::Scalar(255, 0, 0), 2);
            cv::putText(output, shapeName, center - cv::Point(20, -10),
                cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 1);
        }
    }

    // Показ результата
    cv::imshow("Contours", output);
    cv::waitKey(0);
    return 0;
}