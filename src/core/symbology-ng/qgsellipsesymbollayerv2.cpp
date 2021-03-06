/***************************************************************************
 qgsellipsesymbollayerv2.cpp
 ---------------------
 begin                : June 2011
 copyright            : (C) 2011 by Marco Hugentobler
 email                : marco dot hugentobler at sourcepole dot ch
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "qgsellipsesymbollayerv2.h"
#include "qgsdxfexport.h"
#include "qgsexpression.h"
#include "qgsfeature.h"
#include "qgsrendercontext.h"
#include "qgsvectorlayer.h"
#include "qgsdatadefined.h"
#include "qgslogger.h"

#include <QPainter>
#include <QSet>
#include <QDomDocument>
#include <QDomElement>

QgsEllipseSymbolLayerV2::QgsEllipseSymbolLayerV2(): mSymbolName( "circle" ), mSymbolWidth( 4 ), mSymbolWidthUnit( QgsSymbolV2::MM ), mSymbolHeight( 3 ),
    mSymbolHeightUnit( QgsSymbolV2::MM ), mFillColor( Qt::white ), mOutlineColor( Qt::black ), mOutlineStyle( Qt::SolidLine ), mOutlineWidth( 0 ), mOutlineWidthUnit( QgsSymbolV2::MM )
{
  mPen.setColor( mOutlineColor );
  mPen.setStyle( mOutlineStyle );
  mPen.setWidth( 1.0 );
  mPen.setJoinStyle( Qt::MiterJoin );
  mBrush.setColor( mFillColor );
  mBrush.setStyle( Qt::SolidPattern );
  mOffset = QPointF( 0, 0 );

  mAngle = 0;
}

QgsEllipseSymbolLayerV2::~QgsEllipseSymbolLayerV2()
{
}

QgsSymbolLayerV2* QgsEllipseSymbolLayerV2::create( const QgsStringMap& properties )
{
  QgsEllipseSymbolLayerV2* layer = new QgsEllipseSymbolLayerV2();
  if ( properties.contains( "symbol_name" ) )
  {
    layer->setSymbolName( properties[ "symbol_name" ] );
  }
  if ( properties.contains( "symbol_width" ) )
  {
    layer->setSymbolWidth( properties["symbol_width"].toDouble() );
  }
  if ( properties.contains( "symbol_width_unit" ) )
  {
    layer->setSymbolWidthUnit( QgsSymbolLayerV2Utils::decodeOutputUnit( properties["symbol_width_unit"] ) );
  }
  if ( properties.contains( "symbol_width_map_unit_scale" ) )
  {
    layer->setSymbolWidthMapUnitScale( QgsSymbolLayerV2Utils::decodeMapUnitScale( properties["symbol_width_map_unit_scale"] ) );
  }
  if ( properties.contains( "symbol_height" ) )
  {
    layer->setSymbolHeight( properties["symbol_height"].toDouble() );
  }
  if ( properties.contains( "symbol_height_unit" ) )
  {
    layer->setSymbolHeightUnit( QgsSymbolLayerV2Utils::decodeOutputUnit( properties["symbol_height_unit"] ) );
  }
  if ( properties.contains( "symbol_height_map_unit_scale" ) )
  {
    layer->setSymbolHeightMapUnitScale( QgsSymbolLayerV2Utils::decodeMapUnitScale( properties["symbol_height_map_unit_scale"] ) );
  }
  if ( properties.contains( "angle" ) )
  {
    layer->setAngle( properties["angle"].toDouble() );
  }
  if ( properties.contains( "outline_style" ) )
  {
    layer->setOutlineStyle( QgsSymbolLayerV2Utils::decodePenStyle( properties["outline_style"] ) );
  }
  else if ( properties.contains( "line_style" ) )
  {
    layer->setOutlineStyle( QgsSymbolLayerV2Utils::decodePenStyle( properties["line_style"] ) );
  }
  if ( properties.contains( "outline_width" ) )
  {
    layer->setOutlineWidth( properties["outline_width"].toDouble() );
  }
  else if ( properties.contains( "line_width" ) )
  {
    layer->setOutlineWidth( properties["line_width"].toDouble() );
  }
  if ( properties.contains( "outline_width_unit" ) )
  {
    layer->setOutlineWidthUnit( QgsSymbolLayerV2Utils::decodeOutputUnit( properties["outline_width_unit"] ) );
  }
  else if ( properties.contains( "line_width_unit" ) )
  {
    layer->setOutlineWidthUnit( QgsSymbolLayerV2Utils::decodeOutputUnit( properties["line_width_unit"] ) );
  }
  if ( properties.contains( "outline_width_map_unit_scale" ) )
  {
    layer->setOutlineWidthMapUnitScale( QgsSymbolLayerV2Utils::decodeMapUnitScale( properties["outline_width_map_unit_scale"] ) );
  }
  if ( properties.contains( "fill_color" ) )
  {
    //pre 2.5 projects used "fill_color"
    layer->setFillColor( QgsSymbolLayerV2Utils::decodeColor( properties["fill_color"] ) );
  }
  else if ( properties.contains( "color" ) )
  {
    layer->setFillColor( QgsSymbolLayerV2Utils::decodeColor( properties["color"] ) );
  }
  if ( properties.contains( "outline_color" ) )
  {
    layer->setOutlineColor( QgsSymbolLayerV2Utils::decodeColor( properties["outline_color"] ) );
  }
  else if ( properties.contains( "line_color" ) )
  {
    layer->setOutlineColor( QgsSymbolLayerV2Utils::decodeColor( properties["line_color"] ) );
  }
  if ( properties.contains( "size" ) )
  {
    layer->setSize( properties["size"].toDouble() );
  }
  if ( properties.contains( "size_unit" ) )
  {
    layer->setSizeUnit( QgsSymbolLayerV2Utils::decodeOutputUnit( properties["size_unit"] ) );
  }
  if ( properties.contains( "size_map_unit_scale" ) )
  {
    layer->setSizeMapUnitScale( QgsSymbolLayerV2Utils::decodeMapUnitScale( properties["size_map_unit_scale"] ) );
  }
  if ( properties.contains( "offset" ) )
  {
    layer->setOffset( QgsSymbolLayerV2Utils::decodePoint( properties["offset"] ) );
  }
  if ( properties.contains( "offset_unit" ) )
  {
    layer->setOffsetUnit( QgsSymbolLayerV2Utils::decodeOutputUnit( properties["offset_unit"] ) );
  }
  if ( properties.contains( "offset_map_unit_scale" ) )
  {
    layer->setOffsetMapUnitScale( QgsSymbolLayerV2Utils::decodeMapUnitScale( properties["offset_map_unit_scale"] ) );
  }
  if ( properties.contains( "horizontal_anchor_point" ) )
  {
    layer->setHorizontalAnchorPoint( QgsMarkerSymbolLayerV2::HorizontalAnchorPoint( properties[ "horizontal_anchor_point" ].toInt() ) );
  }
  if ( properties.contains( "vertical_anchor_point" ) )
  {
    layer->setVerticalAnchorPoint( QgsMarkerSymbolLayerV2::VerticalAnchorPoint( properties[ "vertical_anchor_point" ].toInt() ) );
  }

  //data defined properties
  layer->restoreDataDefinedProperties( properties );

  //compatibility with old project file format
  if ( !properties["width_field"].isEmpty() )
  {
    layer->setDataDefinedProperty( "width", new QgsDataDefined( properties["width_field"] ) );
  }
  if ( !properties["height_field"].isEmpty() )
  {
    layer->setDataDefinedProperty( "height", new QgsDataDefined( properties["height_field"] ) );
  }
  if ( !properties["rotation_field"].isEmpty() )
  {
    layer->setDataDefinedProperty( "rotation", new QgsDataDefined( properties["rotation_field"] ) );
  }
  if ( !properties["outline_width_field"].isEmpty() )
  {
    layer->setDataDefinedProperty( "outline_width", new QgsDataDefined( properties[ "outline_width_field" ] ) );
  }
  if ( !properties["fill_color_field"].isEmpty() )
  {
    layer->setDataDefinedProperty( "fill_color", new QgsDataDefined( properties["fill_color_field"] ) );
  }
  if ( !properties["outline_color_field"].isEmpty() )
  {
    layer->setDataDefinedProperty( "outline_color", new QgsDataDefined( properties["outline_color_field"] ) );
  }
  if ( !properties["symbol_name_field"].isEmpty() )
  {
    layer->setDataDefinedProperty( "symbol_name", new QgsDataDefined( properties["symbol_name_field"] ) );
  }

  return layer;
}

void QgsEllipseSymbolLayerV2::renderPoint( const QPointF& point, QgsSymbolV2RenderContext& context )
{
  bool ok;
  if ( hasDataDefinedProperty( "outline_width" ) )
  {
    double width = evaluateDataDefinedProperty( "outline_width", context.feature(), mOutlineWidth ).toDouble();
    width *= QgsSymbolLayerV2Utils::lineWidthScaleFactor( context.renderContext(), mOutlineWidthUnit, mOutlineWidthMapUnitScale );
    mPen.setWidthF( width );
  }
  if ( hasDataDefinedProperty( "outline_style" ) )
  {
    QString styleString = evaluateDataDefinedProperty( "outline_style", context.feature(), QVariant(), &ok ).toString();
    if ( ok )
    {
      Qt::PenStyle style = QgsSymbolLayerV2Utils::decodePenStyle( styleString );
      mPen.setStyle( style );
    }
  }
  if ( hasDataDefinedProperty( "fill_color" ) )
  {
    QString colorString = evaluateDataDefinedProperty( "fill_color", context.feature(), QVariant(), &ok ).toString();
    if ( ok )
      mBrush.setColor( QColor( QgsSymbolLayerV2Utils::decodeColor( colorString ) ) );
  }
  if ( hasDataDefinedProperty( "outline_color" ) )
  {
    QString colorString = evaluateDataDefinedProperty( "outline_color", context.feature(), QVariant(), &ok ).toString();
    if ( ok )
      mPen.setColor( QColor( QgsSymbolLayerV2Utils::decodeColor( colorString ) ) );
  }
  double scaledWidth = mSymbolWidth;
  double scaledHeight = mSymbolHeight;
  if ( hasDataDefinedProperty( "width" ) || hasDataDefinedProperty( "height" ) || hasDataDefinedProperty( "symbol_name" ) )
  {
    QString symbolName =  mSymbolName;
    if ( hasDataDefinedProperty( "symbol_name" ) )
    {
      symbolName = evaluateDataDefinedProperty( "symbol_name", context.feature(), mSymbolName ).toString();
    }
    preparePath( symbolName, context, &scaledWidth, &scaledHeight, context.feature() );
  }

  //offset
  double offsetX = 0;
  double offsetY = 0;
  markerOffset( context, scaledWidth, scaledHeight, mSymbolWidthUnit, mSymbolHeightUnit, offsetX, offsetY, mSymbolWidthMapUnitScale, mSymbolHeightMapUnitScale );
  QPointF off( offsetX, offsetY );

  QPainter* p = context.renderContext().painter();
  if ( !p )
  {
    return;
  }

  //priority for rotation: 1. data defined symbol level, 2. symbol layer rotation (mAngle)
  double rotation = 0.0;
  if ( hasDataDefinedProperty( "rotation" ) )
  {
    rotation = evaluateDataDefinedProperty( "rotation", context.feature(), mAngle ).toDouble() + mLineAngle;
  }
  else if ( !qgsDoubleNear( mAngle + mLineAngle, 0.0 ) )
  {
    rotation = mAngle + mLineAngle;
  }
  if ( rotation )
    off = _rotatedOffset( off, rotation );

  QMatrix transform;
  transform.translate( point.x() + off.x(), point.y() + off.y() );
  if ( !qgsDoubleNear( rotation, 0.0 ) )
  {
    transform.rotate( rotation );
  }

  p->setPen( mPen );
  p->setBrush( mBrush );
  p->drawPath( transform.map( mPainterPath ) );
}

QString QgsEllipseSymbolLayerV2::layerType() const
{
  return "EllipseMarker";
}

void QgsEllipseSymbolLayerV2::startRender( QgsSymbolV2RenderContext& context )
{
  QgsMarkerSymbolLayerV2::startRender( context ); // get anchor point expressions
  if ( !context.feature() || !hasDataDefinedProperties() )
  {
    preparePath( mSymbolName, context );
  }
  mPen.setColor( mOutlineColor );
  mPen.setStyle( mOutlineStyle );
  mPen.setWidthF( mOutlineWidth * QgsSymbolLayerV2Utils::lineWidthScaleFactor( context.renderContext(), mOutlineWidthUnit, mOutlineWidthMapUnitScale ) );
  mBrush.setColor( mFillColor );
  prepareExpressions( context.fields(), context.renderContext().rendererScale() );
}

void QgsEllipseSymbolLayerV2::stopRender( QgsSymbolV2RenderContext & )
{
}

QgsSymbolLayerV2* QgsEllipseSymbolLayerV2::clone() const
{
  return QgsEllipseSymbolLayerV2::create( properties() );
}

void QgsEllipseSymbolLayerV2::toSld( QDomDocument &doc, QDomElement &element, QgsStringMap props ) const
{
  QDomElement symbolizerElem = doc.createElement( "se:PointSymbolizer" );
  if ( !props.value( "uom", "" ).isEmpty() )
    symbolizerElem.setAttribute( "uom", props.value( "uom", "" ) );
  element.appendChild( symbolizerElem );

  // <Geometry>
  QgsSymbolLayerV2Utils::createGeometryElement( doc, symbolizerElem, props.value( "geom", "" ) );

  writeSldMarker( doc, symbolizerElem, props );
}

void QgsEllipseSymbolLayerV2::writeSldMarker( QDomDocument &doc, QDomElement &element, QgsStringMap props ) const
{
  // <Graphic>
  QDomElement graphicElem = doc.createElement( "se:Graphic" );
  element.appendChild( graphicElem );

  QgsSymbolLayerV2Utils::wellKnownMarkerToSld( doc, graphicElem, mSymbolName, mFillColor, mOutlineColor, mOutlineStyle, mOutlineWidth, mSymbolWidth );

  // store w/h factor in a <VendorOption>
  double widthHeightFactor = mSymbolWidth / mSymbolHeight;
  QDomElement factorElem = QgsSymbolLayerV2Utils::createVendorOptionElement( doc, "widthHeightFactor", QString::number( widthHeightFactor ) );
  graphicElem.appendChild( factorElem );

  // <Rotation>
  QgsDataDefined* ddRotation = getDataDefinedProperty( "rotation" );

  QString angleFunc = props.value( "angle", "" );
  if ( angleFunc.isEmpty() )  // symbol has no angle set
  {
    if ( ddRotation && ddRotation->isActive() )
    {
      angleFunc = ddRotation->useExpression() ? ddRotation->expressionString() : ddRotation->field();
    }
    else if ( !qgsDoubleNear( mAngle, 0.0 ) )
      angleFunc = QString::number( mAngle );
  }
  else if ( ddRotation && ddRotation->isActive() )
  {
    // the symbol has an angle and the symbol layer have a rotation
    // property set
    angleFunc = QString( "%1 + %2" ).arg( angleFunc ).arg( ddRotation->useExpression() ? ddRotation->expressionString() : ddRotation->field() );
  }
  else if ( !qgsDoubleNear( mAngle, 0.0 ) )
  {
    // both the symbol and the symbol layer have angle value set
    bool ok;
    double angle = angleFunc.toDouble( &ok );
    if ( !ok )
    {
      // its a string (probably a property name or a function)
      angleFunc = QString( "%1 + %2" ).arg( angleFunc ).arg( mAngle );
    }
    else if ( !qgsDoubleNear( angle + mAngle, 0.0 ) )
    {
      // it's a double value
      angleFunc = QString::number( angle + mAngle );
    }
  }
  QgsSymbolLayerV2Utils::createRotationElement( doc, graphicElem, angleFunc );
}

QgsSymbolLayerV2* QgsEllipseSymbolLayerV2::createFromSld( QDomElement &element )
{
  QgsDebugMsg( "Entered." );

  QDomElement graphicElem = element.firstChildElement( "Graphic" );
  if ( graphicElem.isNull() )
    return NULL;

  QString name = "circle";
  QColor fillColor, borderColor;
  double borderWidth, size;
  double widthHeightFactor = 1.0;
  Qt::PenStyle borderStyle;

  QgsStringMap vendorOptions = QgsSymbolLayerV2Utils::getVendorOptionList( graphicElem );
  for ( QgsStringMap::iterator it = vendorOptions.begin(); it != vendorOptions.end(); ++it )
  {
    if ( it.key() == "widthHeightFactor" )
    {
      bool ok;
      double v = it.value().toDouble( &ok );
      if ( ok && !qgsDoubleNear( v, 0.0 ) && v > 0 )
        widthHeightFactor = v;
    }
  }

  if ( !QgsSymbolLayerV2Utils::wellKnownMarkerFromSld( graphicElem, name, fillColor, borderColor, borderStyle, borderWidth, size ) )
    return NULL;

  double angle = 0.0;
  QString angleFunc;
  if ( QgsSymbolLayerV2Utils::rotationFromSldElement( graphicElem, angleFunc ) )
  {
    bool ok;
    double d = angleFunc.toDouble( &ok );
    if ( ok )
      angle = d;
  }

  QgsEllipseSymbolLayerV2 *m = new QgsEllipseSymbolLayerV2();
  m->setSymbolName( name );
  m->setFillColor( fillColor );
  m->setOutlineColor( borderColor );
  m->setOutlineStyle( borderStyle );
  m->setOutlineWidth( borderWidth );
  m->setSymbolWidth( size );
  m->setSymbolHeight( size / widthHeightFactor );
  m->setAngle( angle );
  return m;
}

QgsStringMap QgsEllipseSymbolLayerV2::properties() const
{
  QgsStringMap map;
  map["symbol_name"] = mSymbolName;
  map["symbol_width"] = QString::number( mSymbolWidth );
  map["symbol_width_unit"] = QgsSymbolLayerV2Utils::encodeOutputUnit( mSymbolWidthUnit );
  map["symbol_width_map_unit_scale"] = QgsSymbolLayerV2Utils::encodeMapUnitScale( mSymbolWidthMapUnitScale );
  map["symbol_height"] = QString::number( mSymbolHeight );
  map["symbol_height_unit"] = QgsSymbolLayerV2Utils::encodeOutputUnit( mSymbolHeightUnit );
  map["symbol_height_map_unit_scale"] = QgsSymbolLayerV2Utils::encodeMapUnitScale( mSymbolHeightMapUnitScale );
  map["angle"] = QString::number( mAngle );
  map["outline_style"] = QgsSymbolLayerV2Utils::encodePenStyle( mOutlineStyle );
  map["outline_width"] = QString::number( mOutlineWidth );
  map["outline_width_unit"] = QgsSymbolLayerV2Utils::encodeOutputUnit( mOutlineWidthUnit );
  map["outline_width_map_unit_scale"] = QgsSymbolLayerV2Utils::encodeMapUnitScale( mOutlineWidthMapUnitScale );
  map["color"] = QgsSymbolLayerV2Utils::encodeColor( mFillColor );
  map["outline_color"] = QgsSymbolLayerV2Utils::encodeColor( mOutlineColor );
  map["offset"] = QgsSymbolLayerV2Utils::encodePoint( mOffset );
  map["offset_unit"] = QgsSymbolLayerV2Utils::encodeOutputUnit( mOffsetUnit );
  map["offset_map_unit_scale"] = QgsSymbolLayerV2Utils::encodeMapUnitScale( mOffsetMapUnitScale );
  map["size"] = QString::number( mSize );
  map["size_unit"] = QgsSymbolLayerV2Utils::encodeOutputUnit( mSizeUnit );
  map["size_map_unit_scale"] = QgsSymbolLayerV2Utils::encodeMapUnitScale( mSizeMapUnitScale );
  map["horizontal_anchor_point"] = QString::number( mHorizontalAnchorPoint );
  map["vertical_anchor_point"] = QString::number( mVerticalAnchorPoint );
  saveDataDefinedProperties( map );
  return map;
}

void QgsEllipseSymbolLayerV2::preparePath( const QString& symbolName, QgsSymbolV2RenderContext& context, double* scaledWidth, double* scaledHeight, const QgsFeature* f )
{
  mPainterPath = QPainterPath();
  const QgsRenderContext& ct = context.renderContext();

  double width = 0;

  if ( hasDataDefinedProperty( "width" ) ) //1. priority: data defined setting on symbol layer le
  {
    width = evaluateDataDefinedProperty( "width", f, mSymbolWidth ).toDouble();
  }
  else if ( context.renderHints() & QgsSymbolV2::DataDefinedSizeScale ) //2. priority: is data defined size on symbol level
  {
    width = mSize;
  }
  else //3. priority: global width setting
  {
    width = mSymbolWidth;
  }
  if ( scaledWidth )
  {
    *scaledWidth = width;
  }
  width *= QgsSymbolLayerV2Utils::lineWidthScaleFactor( ct, mSymbolWidthUnit, mSymbolHeightMapUnitScale );

  double height = 0;
  if ( hasDataDefinedProperty( "height" ) ) //1. priority: data defined setting on symbol layer level
  {
    height = evaluateDataDefinedProperty( "height", f, mSymbolHeight ).toDouble();
  }
  else if ( context.renderHints() & QgsSymbolV2::DataDefinedSizeScale ) //2. priority: is data defined size on symbol level
  {
    height = mSize;
  }
  else //3. priority: global height setting
  {
    height = mSymbolHeight;
  }
  if ( scaledHeight )
  {
    *scaledHeight = height;
  }
  height *= QgsSymbolLayerV2Utils::lineWidthScaleFactor( ct, mSymbolHeightUnit, mSymbolHeightMapUnitScale );

  if ( symbolName == "circle" )
  {
    mPainterPath.addEllipse( QRectF( -width / 2.0, -height / 2.0, width, height ) );
  }
  else if ( symbolName == "rectangle" )
  {
    mPainterPath.addRect( QRectF( -width / 2.0, -height / 2.0, width, height ) );
  }
  else if ( symbolName == "cross" )
  {
    mPainterPath.moveTo( 0, -height / 2.0 );
    mPainterPath.lineTo( 0, height / 2.0 );
    mPainterPath.moveTo( -width / 2.0, 0 );
    mPainterPath.lineTo( width / 2.0, 0 );
  }
  else if ( symbolName == "triangle" )
  {
    mPainterPath.moveTo( 0, -height / 2.0 );
    mPainterPath.lineTo( -width / 2.0, height / 2.0 );
    mPainterPath.lineTo( width / 2.0, height / 2.0 );
    mPainterPath.lineTo( 0, -height / 2.0 );
  }
}

void QgsEllipseSymbolLayerV2::setOutputUnit( QgsSymbolV2::OutputUnit unit )
{
  QgsMarkerSymbolLayerV2::setOutputUnit( unit );
  mSymbolWidthUnit = unit;
  mSymbolHeightUnit = unit;
  mOutlineWidthUnit = unit;
}

QgsSymbolV2::OutputUnit QgsEllipseSymbolLayerV2::outputUnit() const
{
  QgsSymbolV2::OutputUnit unit = QgsMarkerSymbolLayerV2::outputUnit();
  if ( mSymbolWidthUnit != unit || mSymbolHeightUnit != unit || mOutlineWidthUnit != unit )
  {
    return QgsSymbolV2::Mixed;
  }
  return unit;
}

void QgsEllipseSymbolLayerV2::setMapUnitScale( const QgsMapUnitScale &scale )
{
  QgsMarkerSymbolLayerV2::setMapUnitScale( scale );
  mSymbolWidthMapUnitScale = scale;
  mSymbolHeightMapUnitScale = scale;
  mOutlineWidthMapUnitScale = scale;
}

QgsMapUnitScale QgsEllipseSymbolLayerV2::mapUnitScale() const
{
  if ( QgsMarkerSymbolLayerV2::mapUnitScale() == mSymbolWidthMapUnitScale &&
       mSymbolWidthMapUnitScale == mSymbolHeightMapUnitScale &&
       mSymbolHeightMapUnitScale == mOutlineWidthMapUnitScale )
  {
    return mSymbolWidthMapUnitScale;
  }
  return QgsMapUnitScale();
}

bool QgsEllipseSymbolLayerV2::writeDxf( QgsDxfExport& e, double mmMapUnitScaleFactor, const QString& layerName, const QgsSymbolV2RenderContext* context, const QgsFeature* f, const QPointF& shift ) const
{
  //width
  double symbolWidth = mSymbolWidth;

  if ( hasDataDefinedProperty( "width" ) ) //1. priority: data defined setting on symbol layer le
  {
    symbolWidth = evaluateDataDefinedProperty( "width", f, mSymbolWidth ).toDouble();
  }
  else if ( context->renderHints() & QgsSymbolV2::DataDefinedSizeScale ) //2. priority: is data defined size on symbol level
  {
    symbolWidth = mSize;
  }
  if ( mSymbolWidthUnit == QgsSymbolV2::MM )
  {
    symbolWidth *= mmMapUnitScaleFactor;
  }

  //height
  double symbolHeight = mSymbolHeight;
  if ( hasDataDefinedProperty( "height" ) ) //1. priority: data defined setting on symbol layer level
  {
    symbolHeight = evaluateDataDefinedProperty( "height", f, mSymbolHeight ).toDouble();
  }
  else if ( context->renderHints() & QgsSymbolV2::DataDefinedSizeScale ) //2. priority: is data defined size on symbol level
  {
    symbolHeight = mSize;
  }
  if ( mSymbolHeightUnit == QgsSymbolV2::MM )
  {
    symbolHeight *= mmMapUnitScaleFactor;
  }

  //outline width
  double outlineWidth = mOutlineWidth;

  if ( hasDataDefinedProperty( "outline_width" ) )
  {
    outlineWidth = evaluateDataDefinedProperty( "outline_width", f, mOutlineWidth ).toDouble();
  }
  if ( mOutlineWidthUnit == QgsSymbolV2::MM )
  {
    outlineWidth *= outlineWidth;
  }

  //fill color
  bool ok;
  QColor fc = mFillColor;
  if ( hasDataDefinedProperty( "fill_color" ) )
  {
    QString colorString = evaluateDataDefinedProperty( "fill_color", f, QVariant(), &ok ).toString();
    if ( ok )
      fc = QColor( colorString );
  }

  //outline color
  QColor oc = mOutlineColor;
  if ( hasDataDefinedProperty( "outline_color" ) )
  {
    QString colorString = evaluateDataDefinedProperty( "outline_color", f, QVariant(), &ok ).toString();
    if ( ok )
      oc = QColor( colorString );
  }

  //symbol name
  QString symbolName = mSymbolName;
  if ( hasDataDefinedProperty( "symbol_name" ) )
  {
    symbolName = evaluateDataDefinedProperty( "symbol_name", f, mSymbolName ).toString();
  }

  //offset
  double offsetX = 0;
  double offsetY = 0;
  markerOffset( *context, offsetX, offsetY );
  QPointF off( offsetX, offsetY );

  //priority for rotation: 1. data defined symbol level, 2. symbol layer rotation (mAngle)
  double rotation = 0.0;
  if ( hasDataDefinedProperty( "rotation" ) )
  {
    rotation = evaluateDataDefinedProperty( "rotation", f, mAngle ).toDouble() + mLineAngle;
  }
  else if ( !qgsDoubleNear( mAngle + mLineAngle, 0.0 ) )
  {
    rotation = mAngle + mLineAngle;
  }
  rotation = -rotation; //rotation in Qt is counterclockwise
  if ( rotation )
    off = _rotatedOffset( off, rotation );

  QTransform t;
  t.translate( shift.x() + offsetX, shift.y() + offsetY );

  if ( rotation != 0 )
    t.rotate( rotation );

  double halfWidth = symbolWidth / 2.0;
  double halfHeight = symbolHeight / 2.0;

  if ( symbolName == "circle" )
  {
    if ( qgsDoubleNear( halfWidth, halfHeight ) )
    {
      QPointF pt( t.map( QPointF( 0, 0 ) ) );
      e.writeFilledCircle( layerName, oc, pt, halfWidth );
    }
    else
    {
      QgsPolyline line;
      double stepsize = 2 * M_PI / 40;
      for ( int i = 0; i < 39; ++i )
      {
        double angle = stepsize * i;
        double x = halfWidth * cos( angle );
        double y = halfHeight * sin( angle );
        QPointF pt( t.map( QPointF( x, y ) ) );
        line.push_back( pt );
      }
      //close ellipse with first point
      line.push_back( line.at( 0 ) );
      e.writePolyline( line, layerName, "SOLID", oc, outlineWidth );
    }
  }
  else if ( symbolName == "rectangle" )
  {
    QPointF pt1( t.map( QPointF( -halfWidth, -halfHeight ) ) );
    QPointF pt2( t.map( QPointF( halfWidth, -halfHeight ) ) );
    QPointF pt3( t.map( QPointF( -halfWidth, halfHeight ) ) );
    QPointF pt4( t.map( QPointF( halfWidth, halfHeight ) ) );
    e.writeSolid( layerName, fc, pt1, pt2, pt3, pt4 );
    return true;
  }
  else if ( symbolName == "cross" )
  {
    QgsPolyline line1( 2 );
    QPointF pt1( t.map( QPointF( -halfWidth, 0 ) ) );
    QPointF pt2( t.map( QPointF( halfWidth, 0 ) ) );
    line1[0] = pt1;
    line1[1] = pt2;
    e.writePolyline( line1, layerName, "CONTINUOUS", oc, outlineWidth );
    QgsPolyline line2( 2 );
    QPointF pt3( t.map( QPointF( 0, halfHeight ) ) );
    QPointF pt4( t.map( QPointF( 0, -halfHeight ) ) );
    line2[0] = pt3;
    line2[1] = pt4;
    e.writePolyline( line2, layerName, "CONTINUOUS", oc, outlineWidth );
    return true;
  }
  else if ( symbolName == "triangle" )
  {
    QPointF pt1( t.map( QPointF( -halfWidth, -halfHeight ) ) );
    QPointF pt2( t.map( QPointF( halfWidth, -halfHeight ) ) );
    QPointF pt3( t.map( QPointF( 0, halfHeight ) ) );
    QPointF pt4( t.map( QPointF( 0, halfHeight ) ) );
    e.writeSolid( layerName, fc, pt1, pt2, pt3, pt4 );
    return true;
  }

  return false; //soon...
}


