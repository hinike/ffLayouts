/*
  ==============================================================================

Copyright (c) 2016, Daniel Walz
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this 
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software without 
   specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND 
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE 
OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED 
OF THE POSSIBILITY OF SUCH DAMAGE.

  ==============================================================================

    juce_ak_layout.h
    Created: 21 Feb 2016 9:14:52pm

  ==============================================================================
*/

#ifndef JUCE_AK_LAYOUT_H_INCLUDED
#define JUCE_AK_LAYOUT_H_INCLUDED

#include "juce_ak_layoutItem.h"

/**
 \mainpage
 <h1>Juce Layout</h1>
 
 This JUCE module provides a simple to use layout mechanism. The items to be laid out are LayoutItems.
 Each LayoutItem can be either a sub layout, a spacer to leave space or a component item which refers to a
 juce::Component. When Layout::updateGeometry is called, the calculated bounds of the item is then
 transfered to the component via setBounds.
 @see LayoutItem
 
 <p>
 The basic usage is to add a Layout instance to your parent component where you want to lay the children in.
 This component doesn't need to be the top most component, it also works for sub components.
 Use the Constructor providing a this pointer to the component, so the overall bounds can be used.
 Also if you want to use the helper LabeledComponent it is needed that the created Label can be displayed
 on the owned component.
 @see Layout
 
 <p>
 Add a call to updateGeometry() in your component's resized() method.
 @see Layout::updateGeometry()
 
 */

//==============================================================================
/**
 Layout aligns a bunch of Components in a row. It can be nested to get any kind of layout.
 
 A minimal example to layout three buttons from left to right would look like this:
 
 \code{.cpp}
 class MainContentComponent   : public Component
 {
 public:
     MainContentComponent () : 
         myLayout (Layout::LeftToRight, this),
         button1 ("Button 1"),
         button2 ("Button 2"),
         button3 ("Button 3")
     {
         addAndMakeVisible (button1);
         addAndMakeVisible (button2);
         addAndMakeVisible (button3);
         myLayout.addComponent (&button1);
         myLayout.addComponent (&button2);
         myLayout.addComponent (&button3);
     }
 
     void resized() override {
         myLayout.updateGeometry ();
     }
 
 private:
     Layout myLayout;
 
     TextButton button1;
     TextButton button2;
     TextButton button3;
 };
 \endcode

 
 @see Component
 @see LayoutItem
 */

class Layout : public LayoutItem
{
public:
    enum Orientation {
        Unknown = 0,
        LeftToRight,
        TopDown,
        RightToLeft,
        BottomUp,
        //GridLayout
    };
    
    Layout (Orientation o=Unknown, juce::Component* owner=nullptr, Layout* parent=nullptr);
    virtual ~Layout();
    
    /**
     Changes the orientation of the layout
     */
    void setOrientation (const Orientation, juce::UndoManager* undo=nullptr);

    /**
     Returns the orientation including direction. If you are only intersted if horizontal or vertical
     @see isHorizontal and @see isVertival
     */
    Orientation getOrientation () const;
    
    static Orientation getOrientationFromName (juce::Identifier name);
    
    static juce::Identifier getNameFromOrientation (Layout::Orientation o);
    
    bool isHorizontal () const;

    bool isVertical ()   const;

    /**
     addComponent creates a LayoutItem to wrap the given Component. To add 
     properties like stretch factor, minimum sizes etc. a pointer to the created
     LayoutItem is returned. You don't need and should not keep this pointer longer 
     than current scope. If you need to alter the item you can access it via @see Layout::getLayoutItem
     */
    virtual LayoutItem* addComponent (juce::Component*, int idx=-1);
    
    // TODO: grid layout
    //virtual LayoutItem* addComponent (Component*, const int x, const int y);
    
    /**
     Remove a component from the layout. The LayoutItem is destructed, but the
     Component is left untouched.
     */
    void removeComponent (juce::Component*);
    
    /**
     Add a component with a label in a sub layout. By chosing the orientation the 
     placement of the label can be set. Either a pointer to a Label pointer can be 
     set to return the created label, or you can call getLabel on the returned LayoutItem.
     */
    LayoutItem* addLabeledComponent (juce::Component*, Orientation, juce::Label** labelPtr=nullptr, int idx=-1);
    

    /**
     Convenience method to ad a labeled component with a given text
     */
    LayoutItem* addLabeledComponent (juce::Component*, juce::StringRef, Orientation o=Layout::TopDown, int idx=-1);
    
    /**
     Creates a nested layout inside a layout.
     */
    Layout* addSubLayout (Orientation, int idx=-1);
    
    /**
     Creates a splitter item to separate a layout manually
     */
    LayoutSplitter* addSplitterItem (float position, int idx=-1);

    /**
     Creates a spacer to put space between items. Use stretch factors to increase
     the space it occupies
     */
    LayoutItem* addSpacer (float sx=1.0, float sy=1.0, int idx=-1);
    
    /**
     Retrieve the LayoutItem for a component. If the Component is not found in the
     Layout, a nullptr is returned.
     */
    LayoutItem* getLayoutItem (juce::Component*);
    
    /**
     Call this method in your Component::resized() callback.
     If the layout has an owning component, this calls updateGeometry with the
     bounds of the owning component.
     */
    virtual void updateGeometry ();
    
    /**
     Recompute the geometry of all components. Recoursively recomputes all sub layouts.
     */
    virtual void updateGeometry (juce::Rectangle<int> bounds);

    /**
     Recompute the geometry of all components. Recoursively recomputes all sub layouts.
     */
    virtual void updateGeometry (juce::Rectangle<int> bounds, int start, int end);
    
    /**
     To show the layout bounds e.g. for debugging yout layout structure simply add the following line to yout Component:
     \code{.cpp}
     void Component::paintOverChildren (Graphics& g) override
     {
         myLayout.paintBounds (g);
     }
     \endcode
     */
    virtual void paintBounds (juce::Graphics& g) const;

    /**
     Cummulates all stretch factors inside the nested layout
     
     Along the orientation the factors are summed up. In the other
     dimension the maximum of the stretch factors is returned.
     */
    void getStretch (float& w, float& h) const override;

    
    /**
     Cummulates all stretch factors inside the nested layout
     */
    void getCummulatedStretch (float& w, float& h, int start=0, int end=-1) const;
    
    /**
     Cummulates size limits of all child items. Along the orientation it sums up
     the minimum sizes and maximum sizes.
     */
    void getSizeLimits (int& minW, int& maxW, int& minH, int& maxH) override;

    /** Clears the layout and resets to zero state */
    void clearLayout (juce::UndoManager* undo=nullptr);

    /**
     Chance for LayoutItems to fix properties that might have changed for saving
     */
    void fixUpLayoutItems () override;
    
    void saveLayoutToValueTree (juce::ValueTree& tree) const override;
    
    /** Return the number of items in the list of items */
    int getNumItems() const { return itemsList.size(); }
    
    /** Return a LayoutItem at a certain index in the list */
    LayoutItem* getLayoutItem (const int idx) { return itemsList.getUnchecked (idx); }
    /** Return a LayoutItem at a certain index in the list */
    const LayoutItem* getLayoutItem (const int idx) const { return itemsList.getUnchecked (idx); }

protected:
    /** This is for internal use only. You should not need to call this method */
    void addRawItem (LayoutItem* item, int idx=-1);

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Layout)
    
    juce::OwnedArray<LayoutItem>       itemsList;

    bool isUpdating;
    bool isFixing;
    mutable bool isCummulatingStretch;
    juce::Component::SafePointer<juce::Component> owningComponent;
    
    static const juce::Identifier propOrientation;
    static const juce::Identifier propLayoutBounds;
    
    static const juce::Identifier orientationUnknown;
    static const juce::Identifier orientationLeftToRight;
    static const juce::Identifier orientationTopDown;
    static const juce::Identifier orientationRightToLeft;
    static const juce::Identifier orientationBottomUp;

};


#endif  // JUCE_AK_LAYOUT_H_INCLUDED
