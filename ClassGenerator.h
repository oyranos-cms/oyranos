class ClassGenerator {
  public:
    ClassGenerator( const QString& tmpl, const QString& src, const QString& dst );

    insert( const QString& name, QVariant value );
    insert( const QString& name, QVariantList value );

    setTemplateDir( const QString& dir );
    setSourceDir( const QString& dir );
    setDestinationDir( const QString& dir );

    render();
    render( const QString& templateFile );
    render( const QString& templateFile, const QString& dstDir );

  private:
};
