/**
 * @mainpage
 * @brief CPM: crystal position map。
 *
 * @author Wang Hongxin
 * @date 2024 年 4 月
 *
 * @details 正电子发射断层成像设备(PET)是一种利用正电子与电子湮灭产生的 γ 射线对
 * 人体进行断层扫描成像的医学影像设备。探测器模块是PET设备的核心部件，其性能直接影
 * 响着设备的灵敏度、分辨率、图像质量等关键指标。在组装整机之前，应对探测器模块性
 * 能做完整测试，保证每个模块性能完全合格再进行组装。在整机设备的声明周期内，我们
 * 还应该定期对探测器模块做标定和性能检测，以保证设备的成像质量不会变化。基于以上
 * 需求，本项目开发了一套PET探测器模块质控标定软件。该软件设计目的还包括提供用户友
 * 好的界面和操作流程，使得PET设备的维护人员能够轻松地进行标定和质控操作。
 *
 * 该软件具有以下功能：
 *
 * 图像分割：能够对探测器模块的floodmap图像进行分割。
 *
 * 位置查找表：能够建立探测器模块的位置查找表，将探测器单元的信号位置映射到图像空间中。
 *
 * 能量查找表：能够建立探测器模块的能量查找表，将探测器单元的信号能量映射到图像空间中。
 *
 * 能量分辨率：能够计算探测器模块的能量分辨率，评估其能量分辨能力。
 *
 * 均匀性：能够查看探测器模块中所有闪烁晶体的均匀性，生成均匀性查找表。
 *
 * 本软件使用C++开发，图形框架使用 QT 5.15.2版本，调用开源计算机视觉库OpenCV 4.5.5版
 * 本，调用GNU开源数学库GSL 2.7版本。
 */

#include <QApplication>
#include <QString>
#include <QDir>
#include <QMap>
#include <QFile>
#include <QtDebug>
#include <QScreen>

#include "Parameters.h"
#include "ParameterForm.h"

int nCM; /**< CM number */
int nBK; /**< BK number per CM */
int nPixel; /**< pixel number of floodmap width/height, default is 512 */
int nCrystal; /**< crystal number of LYSO array width/height, B4 is 26-28 */
int crystalNum; /**< total crystal number, equls to nCrystyal * nCrystal */

qreal enlarge; /**< floodmap enlarge factor */
int bias; /**< floodmap bias value */

int xmin; /**< segment area size: x min */
int xmax; /**< segment area size: x max */
int ymin; /**< segment area size: y min */
int ymax; /**< segment area size: y max */

qreal EW_width; /**< ADC能窗大小，默认30% */

SegmentMethod segMethod; /**< segment method: 1, SVD method; 2, find maximum method */

qreal peakE; /**< ADC峰位对应的真实能量，单位 keV */

int ADC_min; /**< ADC histogram minimum */
int ADC_max; /**< ADC histogram maximum */
int ADC_nBins; /**< ADC histogram bin number */
qreal ADC_binWidth; /**< ADC histogram bin width */
int ADC_cutValue; /**< ADC histogram cut value to avoid to find wrong peak */

qreal recE_min; /**< rec energy histogram minimum */
qreal recE_max; /**< rec nergy histogram maxinum */
int recE_nBins; /**< rec nergy histogram bin number */
qreal recE_binWidth; /**< rec nergy histogram bin width */
qreal recE_cutValue; /**< rec energy histogram cut value to avoid to find wrong peak */

QString currentPath; /**< current program directory */
QString fName_LUT_P; /**< position loop up table file name */
QString fName_LUT_E; /**< energy loop up table file name */
QString fName_LUT_U; /**< uniformity loop up table file name */


/**
 * @brief myMessageOutput
 * @param type
 * @param context
 * @param msg
 */
void myMessageOutput(QtMsgType type,
                     const QMessageLogContext &context,
                     const QString &msg)
{
    QByteArray localMsg = msg.toLocal8Bit();
    const char *file = context.file ? context.file : "";
    const char *function = context.function ? context.function : "";
    switch (type)
    {
    case QtDebugMsg:
        fprintf(stderr, "Debug: %s (%s:%u, %s)\n",
                localMsg.constData(), file, context.line, function);
        break;
    case QtInfoMsg:
        fprintf(stderr, "Info: %s (%s:%u, %s)\n",
                localMsg.constData(), file, context.line, function);
        break;
    case QtWarningMsg:
        fprintf(stderr, "Warning: %s (%s:%u, %s)\n",
                localMsg.constData(), file, context.line, function);
        break;
    case QtCriticalMsg:
        fprintf(stderr, "Critical: %s (%s:%u, %s)\n",
                localMsg.constData(), file, context.line, function);
        break;
    case QtFatalMsg:
        fprintf(stderr, "Fatal: %s (%s:%u, %s)\n",
                localMsg.constData(), file, context.line, function);
        break;
    }
}


/**
 * @brief main function
 * @param argc
 * @param argv
 * @details 主函数创建data文件夹用来存放数据、过程文件等。
 * 主函数读入参数配置文件(隐藏文件，用户不可见)，如果没有参数配置文件则赋值默认值。
 * 然后开打参数配置窗口，让用户在可视化下修改参数。
 */
int main(int argc, char *argv[])
{
    qInstallMessageHandler(myMessageOutput);

    QApplication app(argc, argv);

    // 获取当前程序目录
    QDir dir(QCoreApplication::applicationDirPath());
    currentPath = dir.absolutePath() + "/";
    qInfo() << "Current path = " << currentPath;

    //判断当前目录中是否存在文件夹"Data/"，如果不存在则创建一个
    QString directoryName = "Data";

    // 拼接目标文件夹的路径
    QString targetDirectoryPath = dir.absoluteFilePath(directoryName);

    // 检查文件夹是否存在，如果不存在则创建文件夹
    if (!dir.exists(directoryName))
    {
        if (dir.mkdir(directoryName))
        {
            qDebug() << "Data directory created successfully: " <<
                targetDirectoryPath;
        }
        else
        {
            qCritical() << "Failed to create data directory: " <<
                targetDirectoryPath;
        }
    }


    // readin parameters
    QString fName = currentPath + ".config.par";
    QFile file(fName);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream in(&file);

        QMap<QString, QString> pars;

        while (!in.atEnd())
        {
            QString line = in.readLine();

            if(line.isEmpty() || line.startsWith("#"))
            {
                continue;
            }

            if(line.contains("#"))
            {
                QStringList ss = line.split("#");
                line = ss[0];
            }

            // 去除首尾空格，并按等号进行分割
            QStringList parts = line.simplified().split("=");

            if (2 == parts.size())
            {
                QString key = parts[0].trimmed(); // 去除键的首尾空格
                QString value = parts[1].trimmed(); // 去除值的首尾空格
                pars[key] = value;
            }
            else
            {
                qWarning() << "Invalid format!";
            }
        }

        nCM = pars["nCM"].toInt();
        nBK = pars["nBK"].toInt();
        nPixel = pars["nPixel"].toInt();
        nCrystal = pars["nCrystal"].toInt();
        crystalNum = nCrystal * nCrystal;

        enlarge = pars["enlarge"].toDouble(); // 调整点阵大小
        bias = pars["bias"].toInt();  // 调整点阵位置

        xmin = pars["xmin"].toInt();
        xmax = pars["xmax"].toInt();
        ymin = pars["ymin"].toInt();
        ymax = pars["ymax"].toInt();

        EW_width = pars["EW_width"].toDouble();

        switch (pars["segmentMethod"].toInt())
        {
        case 1:
            segMethod = SegmentMethod::SVD;
            break;
        case 2:
            segMethod = SegmentMethod::FindMaximum;
            break;
        default:
            qDebug() << "SegmentMethod parameter is wrong.";
            break;
        }

        peakE = pars["peakE"].toDouble();

        ADC_min = pars["ADC_min"].toInt();
        ADC_max = pars["ADC_max"].toInt(); // ADC channel value
        ADC_nBins = pars["ADC_nBins"].toInt();
        ADC_binWidth = 1.0*(ADC_max-ADC_min)/ADC_nBins;
        ADC_cutValue = pars["ADC_cutValue"].toInt();

        recE_min = pars["recE_min"].toDouble();
        recE_max = pars["recE_max"].toDouble(); // keV
        recE_nBins = pars["recE_nBins"].toInt();
        recE_binWidth = (recE_max-recE_min)/recE_nBins;
        recE_cutValue = pars["recE_cutValue"].toDouble();

        fName_LUT_P = pars["Position LUT"];
        fName_LUT_E = pars["Energy LUT"];
        fName_LUT_U = pars["Uniformity LUT"];

        file.close();
    }
    else
    {
        // 如果不存在参数文件，则直接设默认值
        nCM = 20;
        nBK = 5;
        nPixel = 512;
        nCrystal = 28;
        crystalNum = nCrystal*nCrystal;

        enlarge = 1.06;
        bias = 58;

        xmin = 0;
        xmax = 512;
        ymin = 0;
        ymax = 512;

        EW_width = 0.30;

        segMethod = SegmentMethod::SVD;

        peakE = 511;

        ADC_min = 0;
        ADC_max = 60000;
        ADC_nBins = 600;
        ADC_binWidth = (ADC_max - ADC_min) / ADC_nBins;
        ADC_cutValue = 15000;

        recE_min = 0;
        recE_max = 1600;
        recE_nBins = 800;
        recE_binWidth = (recE_max - recE_min) / recE_nBins;
        recE_cutValue = 200;

        fName_LUT_P = "LUT_Position.bin";
        fName_LUT_E = "LUT_Energy.bin";
        fName_LUT_U = "LUT_Uniformity.bin";
    }

    // create parameters setup window
    ParameterForm* parWindow = new ParameterForm();
    parWindow->setWindowTitle("Parameters Setup Window");
    parWindow->move(50, 50);
    parWindow->show();

    return app.exec();
}
