<?xml version="1.0" encoding="UTF-8" standalone="no"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
 xmlns:office="urn:oasis:names:tc:opendocument:xmlns:office:1.0"
 xmlns:table="urn:oasis:names:tc:opendocument:xmlns:table:1.0"
 xmlns:text="urn:oasis:names:tc:opendocument:xmlns:text:1.0"
 office:version="1.0">
<xsl:output method="xml" omit-xml-declaration="no" indent="no" encoding="UTF-8" version="1.0"/>
	<xsl:strip-space elements="*"/>

	<xsl:variable name="title-line-no">
	<xsl:choose>
	<xsl:when test="starts-with(upper-case(normalize-space(office:document/office:body/office:spreadsheet/table:table[1]/table:table-row[1]/table:table-cell[1])),'NOTE:')">
		<xsl:value-of select="2"/>
	</xsl:when>
	<xsl:otherwise>
		<xsl:value-of select="1"/>
	</xsl:otherwise>
	</xsl:choose>
	</xsl:variable>

	<xsl:template match="office:document">
	<xsl:call-template name="lineBreak"/>
	<body mimetype="vnd.svs.spreadsheet">

		<xsl:for-each select="office:body/office:spreadsheet/table:table[1]"><xsl:call-template name="lineBreak"/>
		<xsl:element name="table"><xsl:attribute name="name"><xsl:value-of select="@table:name"/></xsl:attribute>
			<xsl:attribute name="border">1</xsl:attribute><xsl:call-template name="lineBreak"/>

		<xsl:for-each select="table:table-row">
			<xsl:choose>
			<xsl:when test="@table:number-rows-repeated">
				<xsl:call-template name="eachLineChunk">
				<xsl:with-param name="row-left" select="number(@table:number-rows-repeated)" />
				</xsl:call-template>
			</xsl:when>
			<xsl:otherwise>
				<xsl:call-template name="eachLine"/>
			</xsl:otherwise>
			</xsl:choose>
		</xsl:for-each>










		</xsl:element><xsl:call-template name="lineBreak"/>
		</xsl:for-each>
	</body><xsl:call-template name="lineBreak"/>
	</xsl:template>
	<xsl:template name="lineBreak">
		<xsl:text disable-output-escaping="yes">&#000013;&#000010;</xsl:text>
	</xsl:template>







<xsl:template name="eachLineChunk">
	<xsl:param name="row-left"/>
	<xsl:if test="$row-left &gt; 0">
				<xsl:call-template name="eachLine"/>
				<xsl:call-template name="eachLineChunk">
				<xsl:with-param name="row-left" select="$row-left - 1" />
				</xsl:call-template>
	</xsl:if>
</xsl:template>



<xsl:template name="eachLine">
	<tr><xsl:call-template name="lineBreak"/>
	<xsl:call-template name = "for-each-table-cell">
	<xsl:with-param name="src-index" select="1" />
	<xsl:with-param name="col-index" select="0" />
	</xsl:call-template>
	</tr>
	<xsl:call-template name="lineBreak"/>
</xsl:template>








<xsl:template name="for-each-table-cell">
	<xsl:param name="src-index"/>
	<xsl:param name="col-index"/>
	<xsl:variable name="repeat-table-cell-no">
	<xsl:choose>
	<xsl:when test="table:table-cell[$src-index]/@table:number-columns-repeated">
		<xsl:value-of select="number(table:table-cell[$src-index]/@table:number-columns-repeated)"/>
	</xsl:when>
	<xsl:otherwise>
		<xsl:value-of select="number(1)"/>
	</xsl:otherwise>
	</xsl:choose>
	</xsl:variable>

	<xsl:variable name = "col-index1">
		<xsl:value-of select="$col-index + $repeat-table-cell-no"/>
	</xsl:variable>

	<xsl:if test="$src-index &lt;= count(table:table-cell)">
		<xsl:call-template name = "write-table-cell">
			<xsl:with-param name="src-index" select="$src-index" />
			<xsl:with-param name="col-index" select="$col-index + 1" />
			<xsl:with-param name="repeat-table-cell-no" select="$repeat-table-cell-no" />
		</xsl:call-template>

		<xsl:call-template name = "for-each-table-cell">
			<xsl:with-param name="src-index" select="$src-index + 1" />
			<xsl:with-param name="col-index" select="$col-index1" />
		</xsl:call-template>
	</xsl:if>
</xsl:template>

<xsl:template name="write-table-cell">
	<xsl:param name="src-index"/>
	<xsl:param name="col-index"/>
	<xsl:param name="repeat-table-cell-no"/>

	<xsl:element name="td"><xsl:attribute name="n"><xsl:value-of select="../table:table-row[number($title-line-no)]/table:table-cell[number($col-index)]"/></xsl:attribute>
	<xsl:call-template name="lineBreak"/>
	<xsl:element name="p"><xsl:attribute name="t"><xsl:value-of select="table:table-cell[$src-index]/@office:value-type"/></xsl:attribute>

		<xsl:apply-templates select="table:table-cell[$src-index]"/>

		
	</xsl:element>
	</xsl:element>
	<xsl:call-template name="lineBreak"/>

	<xsl:if test="$repeat-table-cell-no &gt; 1">
		<xsl:call-template name="write-table-cell">
			<xsl:with-param name="src-index" select="$src-index"/>
			<xsl:with-param name="col-index" select="$col-index + 1"/>
			<xsl:with-param name="repeat-table-cell-no" select="$repeat-table-cell-no - 1"/>
		</xsl:call-template>
	</xsl:if>
</xsl:template>

	<xsl:template match="text:s">
		<xsl:call-template name="write-breakable-whitespace">
			<xsl:with-param name="whitespaces" select="@text:c" />
		</xsl:call-template>
	</xsl:template>

	<xsl:template name="write-breakable-whitespace">
		<xsl:param name="whitespaces" />
		<xsl:text disable-output-escaping="yes"> </xsl:text>
		<xsl:if test="$whitespaces &gt; 1">
			<xsl:call-template name="write-breakable-whitespace">
				<xsl:with-param name="whitespaces" select="$whitespaces - 1" />
			</xsl:call-template>
		</xsl:if>
	</xsl:template>






</xsl:stylesheet>