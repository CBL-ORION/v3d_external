#ifndef JACE_PROXY_LOCI_FORMATS_GUI_CACHECOMPONENT_H
#define JACE_PROXY_LOCI_FORMATS_GUI_CACHECOMPONENT_H

#ifndef JACE_OS_DEP_H
#include "jace/os_dep.h"
#endif

#ifndef JACE_NAMESPACE_H
#include "jace/namespace.h"
#endif

#ifndef JACE_JOBJECT_H
#include "jace/proxy/JObject.h"
#endif

#ifndef JACE_JARRAY_H
#include "jace/JArray.h"
#endif

#ifndef JACE_JFIELD_PROXY_H
#include "jace/JFieldProxy.h"
#endif

#ifndef JACE_JCLASSIMPL_H
#include "jace/JClassImpl.h"
#endif

/**
 * The super class for this class.
 *
 */
#ifndef JACE_PROXY_JAVAX_SWING_JPANEL_H
#include "jace/proxy/javax/swing/JPanel.h"
#endif

/**
 * The interfaces implemented by this class.
 *
 */
#ifndef JACE_PROXY_JAVA_AWT_EVENT_ACTIONLISTENER_H
#include "jace/proxy/java/awt/event/ActionListener.h"
#endif

#ifndef JACE_PROXY_LOCI_FORMATS_CACHE_CACHELISTENER_H
#include "jace/proxy/loci/formats/cache/CacheListener.h"
#endif

#ifndef JACE_PROXY_JAVAX_SWING_EVENT_CHANGELISTENER_H
#include "jace/proxy/javax/swing/event/ChangeListener.h"
#endif

/**
 * Classes which this class is fully dependent upon.
 *
 */
#ifndef JACE_PROXY_JAVA_LANG_STRING_H
#include "jace/proxy/java/lang/String.h"
#endif

/**
 * Forward declarations for the classes that this class uses.
 *
 */
BEGIN_NAMESPACE_4( jace, proxy, java, lang )
class String;
END_NAMESPACE_4( jace, proxy, java, lang )

BEGIN_NAMESPACE_5( jace, proxy, loci, formats, cache )
class CacheEvent;
END_NAMESPACE_5( jace, proxy, loci, formats, cache )

BEGIN_NAMESPACE_5( jace, proxy, loci, formats, cache )
class Cache;
END_NAMESPACE_5( jace, proxy, loci, formats, cache )

BEGIN_NAMESPACE_3( jace, proxy, types )
class JVoid;
END_NAMESPACE_3( jace, proxy, types )

BEGIN_NAMESPACE_5( jace, proxy, loci, formats, gui )

/**
 * The Jace C++ proxy class for loci.formats.gui.CacheComponent.
 * Please do not edit this class, as any changes you make will be overwritten.
 * For more information, please refer to the Jace Developer's Guide.
 *
 */
class CacheComponent : public ::jace::proxy::javax::swing::JPanel, public virtual ::jace::proxy::java::awt::event::ActionListener, public virtual ::jace::proxy::loci::formats::cache::CacheListener, public virtual ::jace::proxy::javax::swing::event::ChangeListener
{
public: 

/**
 * CacheComponent
 *
 */
CacheComponent( ::jace::proxy::loci::formats::cache::Cache p0, ::jace::JArray< ::jace::proxy::java::lang::String > p1 );

/**
 * CacheComponent
 *
 */
CacheComponent( ::jace::proxy::loci::formats::cache::Cache p0, ::jace::JArray< ::jace::proxy::java::lang::String > p1, ::jace::proxy::java::lang::String p2 );

/**
 * getCache
 *
 */
::jace::proxy::loci::formats::cache::Cache getCache();

/**
 * dispose
 *
 */
void dispose();

/**
 * cacheUpdated
 *
 */
void cacheUpdated( ::jace::proxy::loci::formats::cache::CacheEvent p0 );

/**
 * The following methods are required to integrate this class
 * with the Jace framework.
 *
 */
CacheComponent( jvalue value );
CacheComponent( jobject object );
CacheComponent( const CacheComponent& object );
CacheComponent( const NoOp& noOp );
virtual const JClass& getJavaJniClass() const throw ( ::jace::JNIException );
static const JClass& staticGetJavaJniClass() throw ( ::jace::JNIException );
};

END_NAMESPACE_5( jace, proxy, loci, formats, gui )

BEGIN_NAMESPACE( jace )

#ifndef PUT_TSDS_IN_HEADER
  template <> ElementProxy< ::jace::proxy::loci::formats::gui::CacheComponent>::ElementProxy( jarray array, jvalue element, int index );
  template <> ElementProxy< ::jace::proxy::loci::formats::gui::CacheComponent>::ElementProxy( const jace::ElementProxy< ::jace::proxy::loci::formats::gui::CacheComponent>& proxy );
#else
  template <> inline ElementProxy< ::jace::proxy::loci::formats::gui::CacheComponent>::ElementProxy( jarray array, jvalue element, int index ) : 
    ::jace::proxy::loci::formats::gui::CacheComponent( element ), Object( NO_OP ), mIndex( index )
  {
    JNIEnv* env = ::jace::helper::attach();
    parent = static_cast<jarray>( ::jace::helper::newGlobalRef( env, array ) );
  }
  template <> inline ElementProxy< ::jace::proxy::loci::formats::gui::CacheComponent>::ElementProxy( const jace::ElementProxy< ::jace::proxy::loci::formats::gui::CacheComponent>& proxy ) : 
    ::jace::proxy::loci::formats::gui::CacheComponent( proxy.getJavaJniObject() ), Object( NO_OP ), mIndex( proxy.mIndex )
  {
    JNIEnv* env = ::jace::helper::attach();
    parent = static_cast<jarray>( ::jace::helper::newGlobalRef( env, proxy.parent ) );
  }
#endif

#ifndef PUT_TSDS_IN_HEADER
  template <> JFieldProxy< ::jace::proxy::loci::formats::gui::CacheComponent>::JFieldProxy( jfieldID fieldID_, jvalue value, jobject parent_ );
  template <> JFieldProxy< ::jace::proxy::loci::formats::gui::CacheComponent>::JFieldProxy( jfieldID fieldID_, jvalue value, jclass parentClass_ );
  template <> JFieldProxy< ::jace::proxy::loci::formats::gui::CacheComponent>::JFieldProxy( const ::jace::JFieldProxy< ::jace::proxy::loci::formats::gui::CacheComponent>& object );
#else
  template <> inline JFieldProxy< ::jace::proxy::loci::formats::gui::CacheComponent>::JFieldProxy( jfieldID fieldID_, jvalue value, jobject parent_ ) : 
    ::jace::proxy::loci::formats::gui::CacheComponent( value ), Object( NO_OP ), fieldID( fieldID_ )
  {
    JNIEnv* env = ::jace::helper::attach();

    if ( parent_ )
      parent = ::jace::helper::newGlobalRef( env, parent_ );
    else
      parent = parent_;

    parentClass = 0;
  }
  template <> inline JFieldProxy< ::jace::proxy::loci::formats::gui::CacheComponent>::JFieldProxy( jfieldID fieldID_, jvalue value, jclass parentClass_ ) : 
    ::jace::proxy::loci::formats::gui::CacheComponent( value ), Object( NO_OP ), fieldID( fieldID_ )
  {
    JNIEnv* env = ::jace::helper::attach();

    parent = 0;
    parentClass = static_cast<jclass>( ::jace::helper::newGlobalRef( env, parentClass_ ) );
  }
  template <> inline JFieldProxy< ::jace::proxy::loci::formats::gui::CacheComponent>::JFieldProxy( const ::jace::JFieldProxy< ::jace::proxy::loci::formats::gui::CacheComponent>& object ) : 
    ::jace::proxy::loci::formats::gui::CacheComponent( object.getJavaJniValue() ), Object( NO_OP )
  {
    fieldID = object.fieldID; 

    if ( object.parent )
    {
      JNIEnv* env = ::jace::helper::attach();
      parent = ::jace::helper::newGlobalRef( env, object.parent );
    }
    else
      parent = 0;

    if ( object.parentClass )
    {
      JNIEnv* env = ::jace::helper::attach();
      parentClass = static_cast<jclass>( ::jace::helper::newGlobalRef( env, object.parentClass ) );
    }
    else
      parentClass = 0;
  }
#endif

END_NAMESPACE( jace )

#endif // #ifndef JACE_PROXY_LOCI_FORMATS_GUI_CACHECOMPONENT_H
