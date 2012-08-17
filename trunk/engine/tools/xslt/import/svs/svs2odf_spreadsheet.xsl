<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
xmlns:office="urn:oasis:names:tc:opendocument:xmlns:office:1.0" 
xmlns:text="urn:oasis:names:tc:opendocument:xmlns:text:1.0" 
xmlns:table="urn:oasis:names:tc:opendocument:xmlns:table:1.0" 
office:version="1.0">
	<xsl:output indent="no" version="1.0" encoding="UTF-8" method="xml" /> 
	<xsl:template match="/">
<office:document office:mimetype="application/vnd.oasis.opendocument.spreadsheet" office:version="1.0">
<office:body>
<office:spreadsheet>
	<xsl:for-each select="body/table">
	<xsl:element name="table:table"><xsl:attribute name="table:name"><xsl:value-of select="@name"/></xsl:attribute>
	<xsl:for-each select="tr">
			<xsl:call-template name="eachLine"/>
	</xsl:for-each>
	</xsl:element>
	</xsl:for-each>
</office:spreadsheet>
</office:body>
</office:document>
</xsl:template>
<xsl:template name="eachLine">
<table:table-row>
<xsl:for-each select="td">
	<xsl:element name="table:table-cell">
	<xsl:attribute name="office:value-type"><xsl:value-of select="p/@t"/></xsl:attribute>
	<xsl:attribute name="office:value"><xsl:value-of select="p"/></xsl:attribute>
	<text:p><xsl:value-of select="p"/></text:p>
	</xsl:element>
</xsl:for-each>
</table:table-row>
</xsl:template>
</xsl:stylesheet>