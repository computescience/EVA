#include "circuitdiagram.h"
#include <QImage>
#include <QPainter>
#include <QDir>

CircuitDiagram::CircuitDiagram(const CircuitModel* model, QWidget *parent) : 
    QWidget(parent),
    circuitModel(model),
    blockMap(),
    BLANK_BLOCK(BLOCK_W, BLOCK_H, QImage::Format_RGB32)
{
    CircuitModel::initializeElementLibrary();
    BLANK_BLOCK.fill(Qt::white);
    setAutoFillBackground(1);
    /// Import icons from folder "elementicons"
    QDir iconDir (QString("elementicons"),QString("*.png"));
    QStringList allIconsFileNames = iconDir.entryList();
    for (int i=0; i<allIconsFileNames.size(); i++){
        QImage iconImg (iconDir.filePath(allIconsFileNames.at(i)));
        if (!iconImg.isNull()){
            iconImages.push_back(iconImg);
            iconNames.push_back(allIconsFileNames.at(i));
        }
    }
}

void CircuitDiagram::paintEvent(QPaintEvent *)
{
    if (!(blockMap.size()&&blockMap.at(0).size())){
        resize (10, 10);
        return;
    }
    
    QPainter painter(this);
    resize (BLOCK_W * blockMap.at(0).size() + MARGIN*2,
            BLOCK_H * blockMap.size() + MARGIN*2);
    
    for (int row=0; row<blockMap.size(); row++){
        for (int col=0; col<blockMap.at(0).size(); col++){
            int X = BLOCK_W*col + MARGIN, Y = BLOCK_H*row + MARGIN;
            switch (blockMap[row][col].block)
            {
            default: painter.drawImage(X, Y, BLANK_BLOCK); break;
            case CircuitModel::DiagramBlock::Wire:
                painter.drawImage(X, Y, iconLib("wire.png"));
                break;
            case CircuitModel::DiagramBlock::MidVertical:
                painter.drawImage(X, Y, iconLib("vertical.png"));
            case CircuitModel::DiagramBlock::TopBranchBeg:
                painter.drawImage(X, Y, iconLib("para_tl.png"));
                break;
            case CircuitModel::DiagramBlock::TopBranchEnd:
                painter.drawImage(X, Y, iconLib("para_tr.png"));
                break;
            case CircuitModel::DiagramBlock::MidBranchBeg:
                painter.drawImage(X, Y, iconLib("para_ml.png"));
                break;
            case CircuitModel::DiagramBlock::MidBranchEnd:
                painter.drawImage(X, Y, iconLib("para_mr.png"));
                break;
            case CircuitModel::DiagramBlock::BotBranchBeg:
                painter.drawImage(X, Y, iconLib("para_bl.png"));
                break;
            case CircuitModel::DiagramBlock::BotBranchEnd:
                painter.drawImage(X, Y, iconLib("para_br.png"));
                break;
            case CircuitModel::DiagramBlock::Element:
                painter.drawImage(X, Y, iconLib(
                                      blockMap[row][col].elem->elem.iconName()));
                break;
            }
        }
    }
    
}
