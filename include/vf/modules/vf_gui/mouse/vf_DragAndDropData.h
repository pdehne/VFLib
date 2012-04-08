// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file README.md for licensing information.

#ifndef __VF_DRAGANDDROPDATA_VFHEADER__
#define __VF_DRAGANDDROPDATA_VFHEADER__

//==============================================================================
/** Flexible, reference counted container for encapsulating drag and drop data.

    This container is designed to be used with the Juce Component drag and drop
    mechanism. To use it, create a new, dynamic DragAndDropData object. Then
    call add() with one or more objects of class type. Components who are
    interested in the drag and drop information will safely convert the data
    passed from Juce to a DragAndDropData if possible.

    When a Component receives a DragAndDropData during a drag and drop operation,
    it calls find() one or more times to determine if the data contains a
    suitable object.

    DragAndDropData is:

    - Convenient. The helper class DragAndDropTarget works with DragAndDropData
      to manage these objects behind the scenes and hide some of the work.

    - Flexible: A DragAndDropData container can hold any objects which can go
      into a STL container (i.e. support copy construction and assignment).

    - Type-safe: objects placed into the container are disposed of correctly.
      If add(), find() are called with a ReferenceCountedObjectPtr, it does the
      right thing.

    In this example we will create a DragAndDropData for an image represented by
    a hyperlink. The container will hold two typed representations: an Image
    with the actual pixels, and a String for the URL.

    First we declare the data types:

    @code

    struct ImageDragData
    {
      Image image;
    };

    struct URLDragData
    {
      String url;
    };

    @endcode

    Now, create a container and put the data inside. Note that both the Image,
    and the String, are reference counted objects:

    @code

    ImageDragData imageData;
    imageData.image = new Image ();

    URLDragData urlData;
    urlData.url = "logo.jpg";

    DragAndDropData::Ptr dragData (new DragAndDropData);

    dragData.add (imageData);
    dragData.add (urlData);

    // Perform the drag
    DragAndDropContainer* container = findDragAndDropContainer (thisComponent);
    container->startDragging (dragData, ...);

    @endcode

    To modify a Component to receive the container, simply derive it from
    DragAndDropTarget, then override the desired members:

    @code

    class MyComponent : public DragAndDropTarget
    {
    public:
      void itemDropped (DragAndDropData const& data,
                        const SourceDetails& dragSourceDetails)
      {
        ImageDragData imageData;
        URLDragData urlData;

        // We prefer the image, see if it's there
        //
        if (data.find (&imageData))
        {
          // Great, work with imageData.image
        }
        // No image, look for a URL
        //
        else if (data.find (&urlData))
        {
          // We can use urlData.url now
        }
        else
        {
          // The container had nothing we are interested in
        }
      }
    }

    @endcode
*/
class DragAndDropData : public VF_JUCE::ReferenceCountedObject
{
public:
#ifndef DOXYGEN
  typedef VF_JUCE::ReferenceCountedObjectPtr <DragAndDropData> Ptr;

  DragAndDropData ();

  ~DragAndDropData ();
#endif

  /** Add an object to the container.

      The container may not already have an object of this type.

      @param object   The object to add. Object must be copy constructible
                      and support assignment.
  */
  template <class Object>
  void add (Object const& object)
  {
    vfassert (! find <Object> ());
    m_items.push_back (*new StoredItem <Object> (object));
  }

  /** Retrieve an object from the container.

      @param[out] pResult  Receives a copy of the object if it exists in the
                           container.
      
      @return              `true` if the object was found.
  */
  template <class Object>
  bool find (Object* pResult = 0) const
  {
    bool found = false;

    for (Items::const_iterator iter = m_items.begin (); iter != m_items.end ();)
    {
      Item const* item = &(*iter++);
      StoredItem <Object> const* stored =
        dynamic_cast <StoredItem <Object> const*> (item);

      if (stored != 0)
      {
        if (pResult)
          *pResult = stored->getObject ();
        found = true;
        break;
      }
    }
    
    return found;
  }

private:
#ifndef DOXYGEN
  class Item;
  typedef List <Item> Items;

  class Item : public Items::Node
  {
  public:
    // Make sure every object has a vtable so we can use dynamic_cast
    virtual ~Item() { }
  };

  template <class Object>
  class StoredItem : public Item
  {
  public:
    explicit StoredItem (Object const& object)
      : m_object (object)
    {
    }

    Object const& getObject () const { return m_object; }
  
  private:
    Object m_object;
  };
#endif

  Items m_items;
};

#endif
