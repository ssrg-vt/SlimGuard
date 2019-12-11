It is generally better to include pictures (graphs and schemas) as PDF in latex
because they are vector graphics so they render really well in the paper PDF.
To get rid of whatever margin a picture in PDF has (we don't want any when
including the picture in the paper), use pdfcrop:
```
pdfcrop pic.pdf pic.pdf
```

