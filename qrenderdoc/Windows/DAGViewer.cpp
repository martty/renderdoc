/******************************************************************************
 * The MIT License (MIT)
 *
 * Copyright (c) 2019-2020 Baldur Karlsson
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 ******************************************************************************/

#include "DAGViewer.h"
#include <QFontDatabase>
#include "Code/QRDUtils.h"
#include "ui_DAGViewer.h"

DAGViewer::DAGViewer(ICaptureContext &ctx, QWidget *parent)
    : QFrame(parent), ui(new Ui::DAGViewer), m_Ctx(ctx)
{
  ui->setupUi(this);
  m_Ctx.AddCaptureViewer(this);

  m_scene = new QGraphicsScene();
  ui->graphicsView->setScene(m_scene);
  ui->graphicsView->setRenderHint(QPainter::Antialiasing, true);
}

DAGViewer::~DAGViewer()
{
  m_Ctx.BuiltinWindowClosed(this);

  m_Ctx.RemoveCaptureViewer(this);
  delete ui;
}

void DAGViewer::OnCaptureClosed()
{
}

void DAGViewer::OnCaptureLoaded()
{
  QFontMetrics fm(m_scene->font());
  rdcarray<DrawcallDescription> tl;
  for(auto &dc : m_Ctx.CurDrawcalls())
  {
    auto block = new Block(dc.name);
    float y = -20;
    bool input = dc.flags == DrawFlags::Present;
    for(auto &o : dc.outputs)
    {
      if(o != ResourceId::Null())
      {
        auto res = m_Ctx.GetResource(o);
        //block->ports.push_back(Port{res->name});
        auto ti = new QGraphicsSimpleTextItem(block);
        ti->setText(res->name);
        ti->setPos(QPointF(input ? -50 : 50-fm.width(res->name), y));
        y += 10.f;
      }
    }
    m_scene->addItem(block);
  }
}

Block::Block(QString label, QGraphicsItem *parent) : QGraphicsPathItem(parent), m_label(label){
  setPen(QPen(Qt::darkGray));
  setBrush(QBrush(Qt::lightGray));
  setFlag(QGraphicsItem::ItemIsMovable);
  setFlag(QGraphicsItem::ItemIsSelectable);
  auto ti = new QGraphicsSimpleTextItem(this);
  ti->setText(label);
  ti->setPos(QPointF(0, -40));
}

void Block::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
  Q_UNUSED(option)
  Q_UNUSED(widget)

  QFontMetrics fm(scene()->font());
  auto _width = fm.width(m_label);
  QPainterPath p;
  p.addRoundedRect(-_width / 2, -50, _width, 100, 2, 2);
  setPath(p);

  if(isSelected())
  {
    painter->setPen(QPen(Qt::yellow));
  }
  else
  {
    painter->setPen(QPen(Qt::darkGray));
  }
  painter->setBrush(brush());
  painter->drawPath(path());
}
