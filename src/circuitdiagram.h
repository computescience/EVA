#ifndef CIRCUITDIAGRAM_H
#define CIRCUITDIAGRAM_H

#include <QWidget>
#include "circuitmodel.h"
class CircuitDiagram : public QWidget
{
    Q_OBJECT
public:
    explicit CircuitDiagram(const CircuitModel *model, QWidget *parent = NULL);
    
signals:
    
public slots:
    void paintEvent(QPaintEvent*) override;
    void updateDiagram(){
        blockMap = circuitModel->generateDiagram();
    }
private:
    const CircuitModel* circuitModel;
    QVector <QVector <CircuitModel::DiagramBlock> > blockMap;
    QVector <QImage> iconImages;
    QVector <QString> iconNames;
    const QImage& iconLib(QString byName) const {
        if (!byName.endsWith(QString(".png"))) byName = byName + ".png";
        int indexName = iconNames.indexOf(byName);
        return indexName==-1 ? iconImages.at(iconNames.indexOf("unspecified.png")) 
                             : iconImages.at(indexName);
    }
    
    QImage BLANK_BLOCK;
    const int BLOCK_W = 32;
    const int BLOCK_H = 32;
    const int MARGIN  = 16;
};

#endif // CIRCUITDIAGRAM_H
